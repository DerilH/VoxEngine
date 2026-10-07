//
// Created by deril on 2/23/26.
//

#include "VoxEngine/render/vulkan/VulkanCommandBuffer.h"
#include "VoxEngine/render/vulkan/VulkanUtil.h"
#include "VoxEngine/render/buffers/IndexBuffer.h"
#include "VoxEngine/render/buffers/VertexBuffer.h"
#include "VoxEngine/render/vulkan/VulkanResourceCast.h"
#include "VoxEngine/render/state/PipelineState.h"
#include "VoxEngine/render/buffers/UniformBuffer.h"
#include "VoxEngine/render/vulkan/VulkanDescriptorSet.h"

VULKAN_NS
    //    VulkanCommandBuffer::CommandBuffer(VkCommandBuffer handle) : VulkanObject(handle) {
    //        VOX_ASSERT_PTR(handle, "Command buffer is nullptr")
    //    }
    //
    //    void VulkanCommandBuffer::reset(VkCommandBufferResetFlags flags) {
    //        VK_CHECK(vkResetCommandBuffer(mHandle, flags), "Cannot reset command buffer");
    //    }
    //
    //    void VulkanCommandBuffer::begin(VkCommandBufferUsageFlags flags) {
    //        VkCommandBufferBeginInfo beginInfo{};
    //
    //        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    //        VK_CHECK(vkBeginCommandBuffer(mHandle, &beginInfo), "Cannot begin command buffer");
    //    }
    //
    //    void VulkanCommandBuffer::end() {
    //        VK_CHECK(vkEndCommandBuffer(mHandle), "Cannot end command buffer");
    //    }

    VkImageMemoryBarrier2 createBarrier(PassTransition transition, Ref<VulkanTexture> texture);

    void VulkanCommandBuffer::begin() {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        mStarted = true;
        VK_CHECK(vkBeginCommandBuffer(mHandle, &beginInfo), "Cannot begin command buffer");
    }

    void VulkanCommandBuffer::reset() {
        VK_CHECK(vkResetCommandBuffer(mHandle, 0), "Cannot reset command buffer");
    }

    void VulkanCommandBuffer::begin(const int flags) {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = flags;
        mStarted = true;
        VK_CHECK(vkBeginCommandBuffer(mHandle, &beginInfo), "Cannot begin command buffer");
    }

    void VulkanCommandBuffer::setViewportState(float x, float y, Extent extent) {
        VOX_ASSERT(mStarted, "Command buffer not started")

        VkViewport viewport{};
        viewport.x = x;
        viewport.y = y;
        viewport.width = extent.width;
        viewport.height = extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(mHandle, 0, 1, &viewport);
    }

    void VulkanCommandBuffer::end() {
        VOX_ASSERT(mStarted, "Command buffer not started")
        VK_CHECK(vkEndCommandBuffer(mHandle), "Cannot end command buffer");
        mStarted = false;
    }

    void VulkanCommandBuffer::setScissor(int x, int y, Extent extent) {
        VOX_ASSERT(mStarted, "Command buffer not started")
        VkRect2D scissor{};
        scissor.extent = toVk(extent);
        scissor.offset = VkOffset2D{x, y};
        vkCmdSetScissor(mHandle, 0, 1, &scissor);
    }

    void VulkanCommandBuffer::beginDrawingTarget(RenderTargetRef target) {
    }

    void VulkanCommandBuffer::endDrawingTarget(RenderTargetRef target) {
    }

    void VulkanCommandBuffer::setBarriers(ArrayView<PassTransition> transitions, ArrayView<TextureRef> textures) {
        VOX_ASSERT(mStarted, "Command buffer not started")
        VOX_ASSERT(textures.size() == transitions.size(), "Texture and transition count mismatch");
        VkImageMemoryBarrier2 barriers[transitions.size()];
        for (int i = 0; i < transitions.size(); ++i) {
            barriers[i] = createBarrier(transitions[i], ResourceCast(textures[i]));
        }
        VkDependencyInfo depInfo{};
        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers = barriers;

        vkCmdPipelineBarrier2(*this, &depInfo);
    }

    void VulkanCommandBuffer::beginRenderPass(ArrayView<AttachmentDesc> attachments, Extent size, bool clear) {
        VOX_ASSERT(mStarted, "Command buffer not started")
        VkRenderingAttachmentInfo attachmentInfo[attachments.size()];

        for (int i = 0; i < attachments.size(); ++i) {
            attachmentInfo[i] = {};
            attachmentInfo[i].sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            attachmentInfo[i].imageView = ResourceCast(attachments[i].texture->getExact())->getView();
            //TODO: change layout to currently used by image
            // auto t = ResourceCast(attachments[i].texture->getExact());
            // if (t->currentTransition == PassTransition::DISCARD_R_SHADER){
                // attachmentInfo[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            // } else {
                attachmentInfo[i].imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            // }
            attachmentInfo[i].loadOp = clear ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
            attachmentInfo[i].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachmentInfo[i].clearValue = {{0, 0.0f, 0.0f, 0.0f}};
        }

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = toVk(size);
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = attachments.size();
        renderingInfo.pColorAttachments = attachmentInfo;
        vkCmdBeginRendering(*this, &renderingInfo);
    }

    void VulkanCommandBuffer::endRenderPass() {
        VOX_ASSERT(mStarted, "Command buffer not started")
        vkCmdEndRendering(*this);
    }

    void VulkanCommandBuffer::bindPipelineState(PipelineStateRef state) {
        VOX_ASSERT(mStarted, "Command buffer not started")
        auto vkState = ResourceCast(state);
        mCurrentLayout = vkState->mLayout;
        vkCmdBindPipeline(*this, VK_PIPELINE_BIND_POINT_GRAPHICS, *vkState);
    }

    void VulkanCommandBuffer::drawIndexed(uint32_t indexCount) {
        VOX_ASSERT(mStarted, "Command buffer not started")
        vkCmdDrawIndexed(*this, indexCount, 1, 0, 0, 0);
    }

    void VulkanCommandBuffer::draw(uint32_t vertexCount) {
        VOX_ASSERT(mStarted, "Command buffer not started")
        vkCmdDraw(*this, vertexCount, 1, 0, 0);
    }

    void VulkanCommandBuffer::bindIndexBuffer(IndexBufferRef buffer) {
        VOX_ASSERT(mStarted, "Command buffer not started")
        vkCmdBindIndexBuffer(mHandle, *ResourceCast(buffer), 0, toVk(buffer->getIndexType()));
    }

    void VulkanCommandBuffer::bindVertexBuffer(VertexBufferRef buffer) {
        VOX_ASSERT(mStarted, "Command buffer not started")
        VkBuffer vertexBuffers[] = {*ResourceCast(buffer)};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(*this, 0, 1, vertexBuffers, offsets);
    }

    void VulkanCommandBuffer::bindUniformBuffer(UniformBufferRef buffer) {
        VOX_ASSERT(mStarted, "Command buffer not started")

        if (auto internalSet = (VulkanDescriptorSet *) buffer->getInternalDescriptorSet()) {
            VkDescriptorSet meshSet = internalSet->getHandle();
            vkCmdBindDescriptorSets(mHandle, VK_PIPELINE_BIND_POINT_GRAPHICS, mCurrentLayout, 1, 1, &meshSet, 0, nullptr);
        }
    }

    VkImageMemoryBarrier2 createBarrier(PassTransition transition, Ref<VulkanTexture> texture) {
        VkImageMemoryBarrier2 imageBarrier{};

        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageBarrier.srcAccessMask = 0;

        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

        imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        imageBarrier.image = (*texture);
        imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBarrier.subresourceRange.baseMipLevel = 0;
        imageBarrier.subresourceRange.levelCount = 1;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = 1;

        switch (transition) {
            case NONE_W_ATTACHMENT:
                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.srcAccessMask = 0;

                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                break;

            case W_ATTACHMENT_R_COPY:
                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;

                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                break;
            case NONE_W_COPY:
                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
                imageBarrier.srcAccessMask = 0;

                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
                break;
            case W_ATTACHMENT_PRESENT:
                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;

                imageBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_2_NONE;
                break;

            case PRESENT_W_ATTACHMENT:
                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.srcAccessMask = VK_ACCESS_2_NONE;
                imageBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
                break;
            case NONE_PRESENT:
                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT;
                imageBarrier.srcAccessMask = VK_ACCESS_2_NONE;
                imageBarrier.dstAccessMask = VK_ACCESS_2_NONE;
                break;
            case DISCARD_W_ATTACHMENT:
                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.srcAccessMask = VK_ACCESS_2_NONE;
                imageBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                break;
            case DISCARD_R_SHADER:
                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;

                imageBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
                break;
            case W_ATTACHMENT_R_SHADER:
                imageBarrier.oldLayout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                imageBarrier.newLayout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageBarrier.srcStageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.dstStageMask  = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
                imageBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
                break;
        }
        texture->currentTransition = transition;
        return imageBarrier;
    }

NS_END
