//
// Created by deril on 2/17/26.
//

#pragma once

#include <VoxCore/Pointers.h>
#include <VoxEngine/resources/assets/ModelAsset.h>
#include <VoxEngine/scene/Scene.h>

#include "RenderTarget.h"
#include "RenderCore.h"
#include "VoxEngine/render/graph/RenderGraph.h"
#include "DrawItem.h"
#include "VoxEngine/resources/assets/MeshAsset.h"
#include "VoxEngine/resources/assets/ShaderAsset.h"

namespace Vox::Scene {
    class RenderableMeshComponent;
}

RENDER_NS
    class Renderer {
        friend class RendererFactory;
    protected:
        Vector<RenderTargetRef > mRenderTargets;
        int mBufferingLevel = 0;
        bool mShouldStop = false;
        RenderGraph* mGraph = nullptr;
        GraphTextureRef color;
        HashMap<PipelineStateDesc, Vector<DrawItem>> mDrawListByStateHash;
        HashMap<PipelineStateDesc, PipelineStateRef > mPipelineStateByHash;
        HashMap<InternedString, RenderMesh*> mMeshes;
        RenderBackend* mBackend;

        explicit Renderer(RenderBackend* backend);

        void drawFrame(RenderTargetRef target);
        void executeGraph(RenderTargetRef viewport, CommandBufferRef cmdBuffer);
    public:
        const RenderAPI backendApi;
    void init();
        void render(HashSet<Ref<Scene::RenderableComponent>> renderable);
        void render(Ref<Scene::RenderableComponent> el);

        void setBuffering(char buffers);

        void addRenderTarget(RenderTargetRef target);

        void stop();

        void createGraph();

        const HashMap<PipelineStateDesc, Vector<DrawItem>>& getDrawLists() const;
        const Vector<DrawItem>& getDrawList(const PipelineStateDesc& state) const;
        void clearDrawList(const PipelineStateDesc& state);
        void draw(Ref<Scene::RenderableMeshComponent> component);

        void draw(Ref<Resources::ShaderAsset> vertexShaderAsset, Ref<Resources::ShaderAsset> fragmentShaderAsset, Resources::ModelAsset *model);

        PipelineStateRef getPipelineState(PipelineStateDesc desc);

        RenderBackend *getBackend() const;
        const Vector<RenderTargetRef>& getRenderTargets() const { return mRenderTargets; }
        const HashMap<InternedString, RenderMesh*>& getMeshes() const { return mMeshes; }

        RenderGraph& getGraph();

        NO_COPY_MOVE_DEFAULT(Renderer);
    };

NS_END
