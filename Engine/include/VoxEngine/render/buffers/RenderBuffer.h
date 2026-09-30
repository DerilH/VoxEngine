//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/RenderResource.h"
#include "VoxEngine/render/CommandBuffer.h"

RENDER_NS
    class RenderBuffer : public RenderResource {
    protected:
        BufferUsage mUsage;

        explicit RenderBuffer(const BufferUsage usage) : mUsage(usage) {
        }

    public:
        virtual uint32_t sizeInBytes() = 0;

        inline BufferUsage getUsage() const { return mUsage; }
    };

NS_END
