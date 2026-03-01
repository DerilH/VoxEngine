//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/vulkan/passes/GeometryPass.h"
#include "VoxEngine/render/vulkan/VulkanRenderer.h"

VULKAN_NS
    void GeometryPass::execute(const RenderContext& context) {

        VkRenderingAttachmentInfo attachments[mWrites.size()];

        for (int i = 0; i < mWrites.size(); ++i) {
            auto target = (VulkanRenderTarget*) mWrites[i].textureHandle->mNative;
            VkRenderingAttachmentInfo colorAttachmentInfo{};
            colorAttachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            colorAttachmentInfo.imageView = ((VulkanRenderTarget*) target)->getImageView();
            colorAttachmentInfo.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            colorAttachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            VkClearValue clearColor = {{0, 0.0f, 0.2f, 0.0f}};
            colorAttachmentInfo.clearValue = clearColor;
            attachments[i] = colorAttachmentInfo;
        }

        VkRenderingInfo renderingInfo{};
        renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
        renderingInfo.renderArea.offset = {0, 0};
        renderingInfo.renderArea.extent = mExtent;
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = mWrites.size();
        renderingInfo.pColorAttachments = attachments;
        vkCmdBeginRendering(*mCmdBuffer, &renderingInfo);

        VulkanState::Get()->pipeline->bind(*mCmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS);

        for (const auto &mesh: VulkanRenderer::mMeshes) {
            mesh->use(*mCmdBuffer);
            vkCmdDrawIndexed(*mCmdBuffer, mesh->getIndexCount(), 1, 0, 0, 0);
        }
        vkCmdEndRendering(*mCmdBuffer);
    }

    GeometryPass::GeometryPass(RenderPassType mType, const Vector<AttachmentDesc>& reads, const Vector<AttachmentDesc>& writes) : RenderPass(mType, reads, writes) {

    }
NS_END
