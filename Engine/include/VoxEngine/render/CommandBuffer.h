//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxEngine/render/passes/PassTransition.h"
#include "VoxCore/containers/ArrayView.h"
#include "AttachmentDesc.h"
#include "VoxCore/math/Extent.h"
#include "VoxEngine/render/state/PipelineStateDesc.h"
#include "RenderResource.h"
#include "Texture.h"
#include "VoxEngine/render/buffers/RenderBuffer.h"

RENDER_NS
    class CommandBuffer : RenderResource {
    protected:
        bool mStarted = false;
    public:
        //STATE
        virtual void reset() = 0;

        virtual void begin() = 0;

        virtual void end() = 0;


        //VIEWPORT
        virtual void setViewportState(float x, float y, Extent extent) = 0;

        virtual void setScissor(float x, float y, Extent extent) = 0;

        virtual void beginDrawingTarget(RenderTargetRef target) = 0;

        virtual void endDrawingTarget(RenderTargetRef target) = 0;

        //COMMON
        virtual void setBarrier(ArrayView<PassTransition> transitions, ArrayView<TextureRef> textures) = 0;

        virtual void beginRenderPass(ArrayView<AttachmentDesc> attachments, Extent size) = 0;

        virtual void endRenderPass() = 0;

        virtual void bindPipelineState(PipelineStateDesc state) = 0;

        //DRAWS
        virtual void drawIndexed(uint32_t indexCount) = 0;

        virtual void draw(uint32_t vertexCount) = 0;

        //BIND
        virtual void bindIndexBuffer(IndexBufferRef buffer) = 0;

        virtual void bindVertexBuffer(VertexBufferRef buffer) = 0;
    };
NS_END
