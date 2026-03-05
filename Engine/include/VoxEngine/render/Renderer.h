//
// Created by deril on 2/17/26.
//

#pragma once

#include "RenderTarget.h"
#include "vulkan/VulkanWindowRenderTarget.h"
#include "RenderCore.h"
#include "VoxEngine/render/graph/RenderGraph.h"

RENDER_NS
    class Renderer {
        friend class RendererFactory;
    protected:
        Vector<RenderTargetRef > mRenderTargets;
        int mBufferingLevel = 0;
        bool mShouldStop = false;
        RenderAPI mBackendApi;
        RenderGraph* mGraph = nullptr;

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

        NO_COPY_MOVE_DEFAULT(Renderer);
    };

NS_END
