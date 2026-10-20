//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/passes/GeometryPass.h"

#include <VoxEngine/render/buffers/UniformBuffer.h>
#include <VoxEngine/render/state/PipelineState.h>
#include <VoxEngine/scene/components/RenderLayer.h>

#include "VoxEngine/render/Renderer.h"

RENDER_NS
    void GeometryPass::execute(RenderContext context) {
        auto resManager = context.renderer->getRenderResourceManager();
        auto align = resManager->getModelMatrixAlignment();
        uint32_t modelIndex = 0;

        for (auto &map: context.renderer->getDrawLists()) {
            auto entry = map.second.find(static_cast<uint8_t>(Scene::RenderLayer::WORLD));
            if (entry == map.second.end()) continue;
            auto state = context.renderer->getPipeline(map.first, &mRenderingState);
            if (!state) {
                LOG_ERROR("{}", state.error().format());
                continue;
            }
            state.value()->bind(context.cmdBuffer);

            context.cmdBuffer->bindGroup(resManager->getCameraGroup());
            auto texGroup = resManager->getMaterialTextureGroup(map.first, state.value(), context.renderer->getResourceManager());
            if (texGroup) {
                context.cmdBuffer->bindGroup(texGroup);
            }

            for (const auto &item: entry->second) {
                uint32_t offset = modelIndex * align;
                resManager->getModelBuffer()->write(&item.modelMat, sizeof(glm::mat4), offset);
                context.cmdBuffer->bindGroup(resManager->getModelGroup(), {offset});
                item.execute(context.cmdBuffer);
                modelIndex++;
            }
        }

        for (auto &map: context.renderer->getDrawLists()) {
            auto entry = map.second.find(static_cast<uint8_t>(Scene::RenderLayer::GIZMO));
            if (entry == map.second.end()) continue;
            auto state = context.renderer->getPipeline(map.first, &mRenderingState);
            if (!state) {
                LOG_ERROR("{}", state.error().format());
                continue;
            }
            state.value()->bind(context.cmdBuffer);

            context.cmdBuffer->bindGroup(resManager->getCameraGroup());

            for (const auto &item: entry->second) {
                uint32_t offset = modelIndex * align;
                resManager->getModelBuffer()->write(&item.modelMat, sizeof(glm::mat4), offset);
                context.cmdBuffer->bindGroup(resManager->getModelGroup(), {offset});

                item.execute(context.cmdBuffer);
                modelIndex++;
            }
        }
    }

    GeometryPass::GeometryPass(RenderPassType mType, const ArrayView<AttachmentDesc> &reads, const ArrayView<AttachmentDesc> &writes) : RenderPass(mType, reads, writes) {
    }

NS_END
