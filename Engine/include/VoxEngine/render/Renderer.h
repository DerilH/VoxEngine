//
// Created by deril on 2/17/26.
//

#pragma once

#include "RenderTarget.h"
#include "RenderCore.h"
#include "VoxEngine/render/graph/RenderGraph.h"
#include "DrawItem.h"
#include "VoxEngine/resources/assets/MeshAsset.h"
#include "VoxEngine/resources/assets/ShaderAsset.h"

RENDER_NS
    class Renderer {
        friend class RendererFactory;
    protected:
        Vector<RenderTargetRef > mRenderTargets;
        int mBufferingLevel = 0;
        bool mShouldStop = false;
        RenderAPI mBackendApi;
        RenderGraph* mGraph = nullptr;
        GraphTextureRef color;
        HashMap<PipelineStateDesc, Vector<DrawItem>> mDrawListByStateHash;
        HashMap<PipelineStateDesc, PipelineStateRef > mPipelineStateByHash;
        HashMap<InternedString, RenderMesh*> mMeshes;

        explicit Renderer(RenderAPI api) : mBackendApi(api) {
        }

    protected:
        void drawFrame(RenderTargetRef target);
        void executeGraph(RenderTargetRef viewport, CommandBufferRef cmdBuffer);
    public:
        void init();

        void renderLoop();

        void setBuffering(char buffers);

        void addRenderTarget(RenderTargetRef target);

        void stop();

        void createGraph();

        const HashMap<PipelineStateDesc, Vector<DrawItem>>& getDrawLists() const;
        const Vector<DrawItem>& getDrawList(const PipelineStateDesc& state) const;
        void clearDrawList(const PipelineStateDesc& state);
        void draw(Resources::ShaderAsset* vertexShader, Resources::ShaderAsset* fragmentShader, Resources::MeshAsset* asset);
        PipelineStateRef getPipelineState(PipelineStateDesc desc);

        NO_COPY_MOVE_DEFAULT(Renderer);
    };

NS_END
