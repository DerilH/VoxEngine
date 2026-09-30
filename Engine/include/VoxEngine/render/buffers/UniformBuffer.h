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
        UniformBinding mBinding;

        UniformBuffer(const UniformBinding binding) : mBinding(binding) {
        }

    public:
        virtual void write(const void* data, uint32_t size) = 0;

        inline void bind(CommandBufferRef cmdBuffer, uint32_t offset) {
            cmdBuffer->bindUniformBuffer(this);
        }

        virtual void* getInternalDescriptorSet() const { return nullptr; }
    };

NS_END
