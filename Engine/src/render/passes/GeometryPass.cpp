//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/passes/GeometryPass.h"

#include <VoxEngine/render/buffers/UniformBuffer.h>
#include <VoxEngine/render/state/PipelineState.h>

#include "VoxEngine/render/Renderer.h"

RENDER_NS
    void GeometryPass::execute(RenderContext context) {
        auto resManager = context.renderer->getRenderResourceManager();
        for (auto &entry: context.renderer->getDrawLists()) {
            auto state = resManager->getPipeline(entry.first);
            state->bind(context.cmdBuffer);

            context.cmdBuffer->bindGroup(resManager->getCameraGroup());
            context.cmdBuffer->bindGroup(resManager->getModelGroup());
            for (const auto &item: entry.second) {
                resManager->getModelBuffer()->write(&item.modelMat, sizeof(glm::mat4));
                item.execute(context.cmdBuffer);
            }
        }
    }

    GeometryPass::GeometryPass(RenderPassType mType, bool clear, const ArrayView<AttachmentDesc> &reads, const ArrayView<AttachmentDesc> &writes) : RenderPass(mType, reads, writes), mShouldClear(clear) {
    }

NS_END
