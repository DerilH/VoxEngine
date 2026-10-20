#pragma once

#include <VoxCore/Pointers.h>
#include <VoxCore/signal/Event.h>
#include <../../../../Editor/src/assets/MaterialAsset.h>
#include <VoxEngine/resources/assets/ModelAsset.h>
#include <VoxEngine/scene/Scene.h>

#include "RenderTarget.h"
#include "RenderCore.h"
#include "VoxEngine/render/graph/RenderGraph.h"
#include "DrawItem.h"
#include "FrameSync.h"
#include "MaterialPipelines.h"
#include "RenderResourceManager.h"
#include "../../../../Editor/src/assets/variants/MeshAsset.h"
#include "../../../../Editor/src/assets/variants/ShaderAsset.h"

namespace Vox::Resources {
    class AssetManager;
}

namespace Vox::Scene {
    class MeshRendererComponent;
}

RENDER_NS
    class WindowRenderTarget;

    class Renderer {
        friend class RendererFactory;

    protected:
        HashMap<InternedString, Ref<RenderTarget> > mViewportTargets;
        HashMap<InternedString, Ref<RenderTarget> > mPresentTargets;
        Ref<FrameSync> mHeadlessFrame;

        int mBufferingLevel = 0;
        RenderGraph *mGraph = nullptr;
        HashMap<ConstRef<Resources::MaterialAsset>, OrderedMap<uint8_t, Vector<DrawItem>>> mLayerDrawListByMaterial;
        HashMap<ConstRef<Resources::MaterialAsset>, MaterialPipelines> mPipelinesByMaterial;

        Ref<RenderBackend> mBackend;
        Ref<RenderResourceManager> mRenderResourceManager;
        Ref<Resources::AssetManager> mResourceManager;

        explicit Renderer(Ref<Resources::AssetManager> resourceManager, Ref<RenderResourceManager> renderResManager, Ref<RenderBackend> backend);

        void drawFrame(RenderTargetRef target, Ref<CommandBuffer> cmdBuffer);

        void executeGraph(RenderTargetRef viewport, CommandBufferRef cmdBuffer);


    public:
        const RenderAPI backendApi;
        Event<Ref<Renderer>> onCollect;

        void init();

        void render(Ref<Scene::Scene> scene);

        void render(Ref<Scene::RendererComponent> el);

        void setBuffering(char buffers);

        void addViewportTarget(InternedString name, RenderTargetRef target);

        void addPresentTarget(InternedString name, Ref<RenderTarget> target);

        void createGraph();

        const HashMap<ConstRef<Resources::MaterialAsset>, OrderedMap<uint8_t, Vector<DrawItem>>> &getDrawLists() const {
            return mLayerDrawListByMaterial;
        }

        const OrderedMap<uint8_t, Vector<DrawItem>> &getLayerDrawList(const ConstRef<Resources::MaterialAsset> &state) const {
            return mLayerDrawListByMaterial.at(state);
        }

        void clearDrawList(ConstRef<Resources::MaterialAsset> state);

        void draw(Ref<Scene::MeshRendererComponent> component);
        void draw(ConstRef<Resources::MeshAsset> mesh, ConstRef<Resources::MaterialAsset> material, glm::mat4 modelMat, uint8_t layer, uint64_t id);
        void updateCamera(Ref<Scene::CameraComponent> camera);

        Expected<Ref<PipelineState> > getPipeline(ConstRef<Resources::MaterialAsset> material, ConstRef<RenderingState> renderingState);

        Ref<RenderBackend> getBackend() const { return mBackend; }
        Ref<Resources::AssetManager> getResourceManager() const { return mResourceManager; }
        Ref<RenderResourceManager> getRenderResourceManager() const { return mRenderResourceManager; }
        const HashMap<InternedString, RenderTargetRef> &getViewportTargets() const { return mViewportTargets; }
        const HashMap<InternedString, Ref<RenderTarget>> & getPresentTargets() const {return mPresentTargets;}

        RenderGraph &getGraph() const { return *mGraph; }

        NO_COPY_MOVE_DEFAULT(Renderer);
    };

NS_END
