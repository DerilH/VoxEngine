//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxEngine/render/vulkan/passes/PassTransition.h"
#include "VoxEngine/render/graph/TextureHandle.h"
#include "VoxCore/containers/Buffer.h"
#include "AttachmentDesc.h"
#include "VoxCore/math/Extent.h"
#include "VoxEngine/render/state/PipelineStateDesc.h"

RENDER_NS
class CommandBuffer {
protected:
    bool mStarted = false;
public:
    //STATE
    virtual void reset() = 0;
    virtual void begin() = 0;
    virtual void end() = 0;

    //COMMON
    virtual void setBarrier(Buffer<PassTransition> transitions, Buffer<TextureHandle> textures) = 0;
    virtual void beginRenderPass(Buffer<AttachmentDesc> attachments, Extent size) = 0;
    virtual void endRenderPass() = 0;
    virtual void bindPipelineState(PipelineStateDesc state) = 0;

    //DRAWS
    virtual void drawIndexed(uint32_t indexCount) = 0;
    virtual void draw(uint32_t vertexCount) = 0;


    virtual void setScissor(float width, float height) = 0;
    virtual void setViewport(float width, float height) = 0;
};
NS_END
