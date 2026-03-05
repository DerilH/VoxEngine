//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "RenderBuffer.h"

RENDER_NS
    class IndexBuffer : public RenderBuffer {
        IndexType mIndexType;
        uint32_t mCount;
    public:
        explicit IndexBuffer(BufferUsage usage, IndexType type, uint32_t count) : RenderBuffer(usage), mIndexType(type), mCount(count) {};

        inline void bind(CommandBufferRef cmdBuffer) override {
            cmdBuffer->bindIndexBuffer(this);
        }

        inline IndexType getIndexType() const { return mIndexType; }

        inline uint32_t getCount() const {return mCount;}
        inline void setCount(uint32_t count) { mCount = count; }
    };

NS_END
