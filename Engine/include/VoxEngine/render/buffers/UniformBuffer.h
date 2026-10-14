//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "RenderBuffer.h"
#include "UniformBinding.h"

RENDER_NS
    class UniformBuffer : virtual public RenderBuffer {
    protected:
        UniformBuffer() {
        }

    public:
        virtual void write(const void *data, uint32_t size) = 0;

        virtual void write(const void *data, uint32_t size, uint32_t offset) = 0;
    };

NS_END
