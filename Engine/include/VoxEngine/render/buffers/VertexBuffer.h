//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "RenderBuffer.h"

RENDER_NS
    class VertexBuffer : virtual public RenderBuffer {
    protected:
        explicit VertexBuffer() {}
    public:
        inline void bind(CommandBufferRef cmdBuffer) {
            cmdBuffer->bindVertexBuffer(this);
        }
    };
NS_END
