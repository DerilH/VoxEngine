//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/vulkan/passes/PresentPass.h"
#include "VoxEngine/render/vulkan/VulkanRenderTarget.h"
#include "VoxEngine/render/vulkan/VulkanState.h"

VULKAN_NS
    PresentPass::PresentPass(RenderPassType mType, const Vector<AttachmentDesc>& reads, const Vector<AttachmentDesc>& writes) : RenderPass(mType, reads, writes) {}

    void PresentPass::execute(const RenderContext& context) {

        VkRenderingAttachmentInfo attachments[mWrites.size()];
        for (int i = 0; i < mWrites.size(); ++i) {
            auto target = (VulkanRenderTarget*) mWrites[i].textureHandle->mNative;
            VkRenderingAttachmentInfo colorAttachmentInfo{};
            colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            colorAttachmentInfo.imageView = target->getImageView();
            colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            VkClearValue clearColor = {{0, 0.0f, 0.0f, 0.0f}};
            colorAttachmentInfo.clearValue = clearColor;
            attachments[i] = colorAttachmentInfo;
        }

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = {400, 400};
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = mWrites.size();
        renderingInfo.pColorAttachments = attachments;

//        vkCmdBeginRendering(*mCmdBuffer, &renderingInfo);
        VkImageCopy c{};
        c.extent = VkExtent3D(mExtent.width,mExtent.height,1);
        auto src = (VulkanRenderTarget*) mReads[0].textureHandle->mNative;;
        auto dst = (VulkanRenderTarget*) mWrites[0].textureHandle->mNative;;
        c.srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        c.dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
        vkCmdCopyImage(*mCmdBuffer,src->getImage(), VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst->getImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,1, &c);

        VkImageMemoryBarrier2 barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

        barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

        barrier.dstStageMask = VK_PIPELINE_STAGE_2_NONE;
        barrier.dstAccessMask = 0;

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        barrier.image = dst->getImage();
        barrier.subresourceRange = {
                VK_IMAGE_ASPECT_COLOR_BIT,
                0, 1,
                0, 1
        };

        VkDependencyInfo dep{};
        dep.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dep.imageMemoryBarrierCount = 1;
        dep.pImageMemoryBarriers = &barrier;

        vkCmdPipelineBarrier2(*mCmdBuffer, &dep);
        barrier = VkImageMemoryBarrier2 {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

        barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
        barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT | VK_ACCESS_2_TRANSFER_WRITE_BIT;

        barrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        barrier.image = src->getImage();
        barrier.subresourceRange = {
                VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1
        };

        dep = VkDependencyInfo {};
        dep.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        dep.imageMemoryBarrierCount = 1;
        dep.pImageMemoryBarriers = &barrier;

        vkCmdPipelineBarrier2(*mCmdBuffer, &dep);
//        vkCmdEndRendering(*mCmdBuffer);
    }

NS_END
