//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/passes/GeometryPass.h"

#include <VoxEngine/render/buffers/UniformBuffer.h>
#include <VoxEngine/render/state/PipelineState.h>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/resources/assets/TextureAsset.h>

#include "VoxEngine/render/Renderer.h"

RENDER_NS
    void GeometryPass::execute(RenderContext context) {
        auto resManager = context.renderer->getRenderResourceManager();
        for (auto &entry: context.renderer->getDrawLists()) {
            auto state = context.renderer->getPipeline(entry.first, &mRenderingState);
            if (!state) {
                LOG_ERROR("Cannot get pipeline state");
                continue;
            }
            state.value()->bind(context.cmdBuffer);

            context.cmdBuffer->bindGroup(resManager->getCameraGroup());
            context.cmdBuffer->bindGroup(resManager->getModelGroup());
            context.cmdBuffer->bindGroup(resManager->getSamplerGroup());

            for (const auto &item: entry.second) {
                static Ref<Texture> text = nullptr;
                if (!text) {
                    auto t = context.renderer->getResourceManager()->get<Resources::TextureAsset>("textures/Wood.png");
                    if (t.has_value()) {
                        text = context.renderer->getBackend()->createTexture(Format::RGBA8, (*t)->getExtent(), (*t)->getPixels().pData, (*t)->getPixels().size());
                    }
                }
                resManager->updateSample(text);
                resManager->getModelBuffer()->write(&item.modelMat, sizeof(glm::mat4));
                item.execute(context.cmdBuffer);
            }
        }
    }

    GeometryPass::GeometryPass(RenderPassType mType, bool clear, const ArrayView<AttachmentDesc> &reads, const ArrayView<AttachmentDesc> &writes) : RenderPass(mType, reads, writes), mShouldClear(clear) {
    }

NS_END
