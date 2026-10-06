//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/passes/GeometryPass.h"

#include <VoxEngine/render/vulkan/VulkanDescriptorSet.h>
#include <VoxEngine/render/vulkan/VulkanResourceCast.h>

#include "VoxEngine/render/Renderer.h"

RENDER_NS
    void GeometryPass::execute(RenderContext context) {
        for (auto &entry: context.renderer->getDrawLists()) {
            auto state = context.renderer->getRenderResourceManager()->getPipeline(entry.first);
            state->bind(context.cmdBuffer);
            for (const auto &item: entry.second) {

                auto vkDevice = Vulkan::ResourceCast(context.renderer->getBackend()->getDevice());

                VkDescriptorSet globalSet = vkDevice->getGlobalDescriptorSet()->getHandle();
                auto l = Vulkan::ResourceCast(context.cmdBuffer)->getCurrentLayout();
                vkCmdBindDescriptorSets(Vulkan::ResourceCast(context.cmdBuffer)->getHandle(), VK_PIPELINE_BIND_POINT_GRAPHICS, l, 0, 1, &globalSet, 0, nullptr);

                item.execute(context.cmdBuffer);
            }
        }
    }

    GeometryPass::GeometryPass(RenderPassType mType, const ArrayView<AttachmentDesc> &reads, const ArrayView<AttachmentDesc> &writes) : RenderPass(mType, reads, writes) {
    }

NS_END
