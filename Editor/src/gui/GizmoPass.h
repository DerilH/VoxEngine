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

        Ref<Render::RenderResourceManager> mResManager = nullptr;
        Ref<Render::UniformBuffer> mIdBuffer = nullptr;
        Ref<Render::BindGroup> mIdBindGroup = nullptr;
        size_t mIdCapacity = 0;

    public:
        Event<uint64_t> onPickComplete;

        GizmoPass(Render::RenderPassType mType, ArrayView<Render::AttachmentDesc> reads, Render::AttachmentDesc idTexture, Render::AttachmentDesc colorTexture)
            : RenderPass(mType, reads, {idTexture}) {
        }

        ~GizmoPass() override {
            if (mIdBuffer && mResManager) {
                mResManager->freeUniformBuffer(mIdBuffer);
                mIdBuffer = nullptr;
            }
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
                mResManager = resManager;
                auto align = resManager->getModelMatrixAlignment();
                auto idAlign = resManager->alignUniformOffset(sizeof(uint64_t));

                size_t totalItems = 0;
                for (auto &map: context.renderer->getDrawLists()) {
                    auto eWorld = map.second.find(static_cast<uint8_t>(Scene::RenderLayer::WORLD));
                    if (eWorld != map.second.end()) totalItems += eWorld->second.size();
                    auto eGizmo = map.second.find(static_cast<uint8_t>(Scene::RenderLayer::GIZMO));
                    if (eGizmo != map.second.end()) totalItems += eGizmo->second.size();
                }

                constexpr size_t MIN_CAPACITY = 4096;
                size_t neededCapacity = std::max(MIN_CAPACITY, totalItems);

                if (!mIdBuffer || mIdCapacity < neededCapacity) {
                    if (mIdBuffer) {
                        resManager->freeUniformBuffer(mIdBuffer);
                    }
                    mIdCapacity = neededCapacity;
                    mIdBuffer = resManager->allocateUniformBuffer(mIdCapacity * idAlign);
                    mIdBindGroup = nullptr;
                }

                uint32_t modelIndex = 0;

                auto bindPipeline = [&](ConstRef<Resources::MaterialAsset> asset) {
                    auto material = asset->cloneTyped();
                    material->shaders[Render::ShaderStage::FRAGMENT] = "editor/IDShader.frag";
                    auto state = context.renderer->getPipeline(material.get(), &mRenderingState);
                    if (!state) {
                        LOG_ERROR("{}", state.error().format());
                        return;
                    }
                    state.value()->bind(context.cmdBuffer);
                    if (!mIdBindGroup) {
                        mIdBindGroup = resManager->createBindGroup(state.value(), 3);
                        resManager->updateBindGroupDynamic(mIdBindGroup, 0, resManager->getModelBuffer(), sizeof(glm::mat4));
                        resManager->updateBindGroupDynamic(mIdBindGroup, 1, mIdBuffer, sizeof(uint64_t));
                    }
                    context.cmdBuffer->bindGroup(resManager->getCameraGroup());
                };

                auto renderList = [&](const Vector<Render::DrawItem> &items) {
                    for (const auto &item: items) {
                        uint32_t offset = modelIndex * align;
                        resManager->getModelBuffer()->write(&item.modelMat, sizeof(glm::mat4), offset);

                        uint32_t offsetId = modelIndex * idAlign;
                        mIdBuffer->write(&item.instanceId, sizeof(uint64_t), offsetId);
                        context.cmdBuffer->bindGroup(mIdBindGroup, {offset, offsetId});

                        item.execute(context.cmdBuffer);
                        modelIndex++;
                    }
                };

                for (auto &map: context.renderer->getDrawLists()) {
                    bindPipeline(map.first);
                    auto it = map.second.find(std::to_underlying(Scene::RenderLayer::WORLD));
                    if (it != map.second.end()) {
                        renderList(it->second);
                    }
                }

                for (auto &map: context.renderer->getDrawLists()) {
                    bindPipeline(map.first);
                    auto it = map.second.find(std::to_underlying(Scene::RenderLayer::GIZMO));
                    if (it != map.second.end()) {
                        renderList(it->second);
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
