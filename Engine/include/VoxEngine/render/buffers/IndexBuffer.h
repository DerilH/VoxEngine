//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "RenderBuffer.h"

RENDER_NS
    class IndexBuffer : virtual public RenderBuffer {
    protected:
        IndexType mIndexType;
        uint32_t mCount;

        explicit IndexBuffer(const IndexType type, const uint32_t count) : mIndexType(type), mCount(count) {};

    public:

        inline void bind(CommandBufferRef cmdBuffer) {
            cmdBuffer->bindIndexBuffer(this);
        }

        inline IndexType getIndexType() const { return mIndexType; }

        inline uint32_t getCount() const { return mCount; }

        inline void setCount(uint32_t count) { mCount = count; }

    };

NS_END
