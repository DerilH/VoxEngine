//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/RenderResource.h"
#include "VoxEngine/render/CommandBuffer.h"

RENDER_NS
    class RenderBuffer : public RenderResource {
        BufferUsage mUsage;
    public:
        RenderBuffer(BufferUsage usage) : mUsage(usage) {}

        virtual void bind(CommandBufferRef cmdBuffer) = 0;

        inline BufferUsage getUsage() const { return mUsage; }
    };
NS_END

