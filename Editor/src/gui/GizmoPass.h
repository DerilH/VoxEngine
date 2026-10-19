//
// Created by deril on 2/27/26.
//

#pragma once
#include <VoxCore/signal/Event.h>
#include <VoxEngine/render/Renderer.h>
#include <VoxEngine/render/buffers/UniformBuffer.h>
#include <VoxEngine/render/passes/RenderPass.h>
#include <VoxEngine/render/state/PipelineState.h>
#include <VoxEngine/resources/ResourceManager.h>
#include <VoxEngine/scene/components/RenderLayer.h>


namespace Vox::Editor {
    class GizmoPass : public Render::RenderPass {
        Ref<Render::TransferBuffer> mTransferBuffer = nullptr;
        bool mSchedulePick = false;
        bool mReadOnNext = false;
        Extent mPickPos{500, 500};

    public:
        Event<uint64_t> onPickComplete;
        GizmoPass(Render::RenderPassType mType, ArrayView<Render::AttachmentDesc> reads, Render::AttachmentDesc idTexture, Render::AttachmentDesc colorTexture) : RenderPass(mType, reads, {idTexture}) {
        }

        void execute(Render::RenderContext context) override {
            //TODO: Replace with submit wait system
            if (mReadOnNext) {
                const auto a = static_cast<uint64_t *>(mTransferBuffer->getSpan(sizeof(uint64_t)).pData);
                onPickComplete.invoke(*a);
                mReadOnNext = false;
            }
            if (mSchedulePick) {
                if (!mTransferBuffer) {
                    mTransferBuffer = context.renderer->getBackend()->createTransferBuffer(sizeof(uint64_t));
                }
                auto resManager = context.renderer->getRenderResourceManager();
                auto align = resManager->getModelMatrixAlignment();
                auto additionalAlign = resManager->getAdditionalAlignment();

                uint32_t modelIndex = 0;

                for (auto &map: context.renderer->getDrawLists()) {
                    auto entry = map.second.find(static_cast<uint8_t>(Scene::RenderLayer::WORLD));
                    if (entry == map.second.end()) continue;
                    auto material = map.first->cloneTyped();
                    material->shaders[Render::ShaderStage::FRAGMENT] = "editor/IDShader.frag";
                    auto state = context.renderer->getPipeline(material.get(), &mRenderingState);
                    if (!state) {
                        LOG_ERROR("{}", state.error().format());
                        continue;
                    }
                    state.value()->bind(context.cmdBuffer);

                    context.cmdBuffer->bindGroup(resManager->getCameraGroup());

                    for (const auto &item: entry->second) {
                        uint32_t offset = modelIndex * align;
                        resManager->getModelBuffer()->write(&item.modelMat, sizeof(glm::mat4), offset);

                        uint32_t offset1 = modelIndex * additionalAlign;
                        resManager->getAdditionalBuffer()->write(&item.instanceId, sizeof(uint64_t), offset);
                        context.cmdBuffer->bindGroup(resManager->getAdditionalGroup(), {offset, offset1});

                        item.execute(context.cmdBuffer);
                        modelIndex++;
                    }
                }

                for (auto &map: context.renderer->getDrawLists()) {
                    auto entry = map.second.find(static_cast<uint8_t>(Scene::RenderLayer::GIZMO));
                    if (entry == map.second.end()) continue;
                    auto material = map.first->cloneTyped();
                    material->shaders[Render::ShaderStage::FRAGMENT] = "editor/IDShader.frag";
                    auto state = context.renderer->getPipeline(material.get(), &mRenderingState);
                    if (!state) {
                        LOG_ERROR("{}", state.error().format());
                        continue;
                    }
                    state.value()->bind(context.cmdBuffer);

                    context.cmdBuffer->bindGroup(resManager->getCameraGroup());

                    for (const auto &item: entry->second) {
                        uint32_t offset = modelIndex * align;
                        resManager->getModelBuffer()->write(&item.modelMat, sizeof(glm::mat4), offset);

                        uint32_t offset1 = modelIndex * additionalAlign;
                        resManager->getAdditionalBuffer()->write(&item.instanceId, sizeof(uint64_t), offset);
                        context.cmdBuffer->bindGroup(resManager->getAdditionalGroup(), {offset, offset1});

                        item.execute(context.cmdBuffer);
                        modelIndex++;
                    }
                }

                context.cmdBuffer->copyTextureToBuffer(mWrites[0].texture->getExact(), mTransferBuffer, mPickPos, Extent(1, 1));
                mSchedulePick = false;
                mReadOnNext = true;


            }

        }

        void schedulePick(bool schedule, Extent pos) {
            mSchedulePick = schedule;
            mPickPos = pos;
        }
    };
}
