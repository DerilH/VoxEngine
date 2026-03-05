//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "RenderBuffer.h"

RENDER_NS
    class VertexBuffer : public RenderBuffer {
    public:
        explicit VertexBuffer(BufferUsage usage) : RenderBuffer(usage) {}

        inline void bind(CommandBufferRef cmdBuffer) override {
            cmdBuffer->bindVertexBuffer(this);
        }
    };
NS_END
