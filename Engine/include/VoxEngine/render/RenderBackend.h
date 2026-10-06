//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/Assert.h"
#include "WindowRenderTarget.h"
#include "Device.h"
#include "VoxCore/render/Enums.h"
#include "RenderCore.h"
#include "RenderCore.h"
#include "VoxEngine/render/state/PipelineStateDesc.h"

RENDER_NS
    class RenderBackend {
    protected:
        DeviceRef mCurrentDevice = nullptr;

        RenderBackend(RenderAPI api) : api(api) {}
        explicit RenderBackend() = delete;
        ~RenderBackend() = default;

    public:
        const RenderAPI api;

        virtual void init() = 0;
        virtual int32_t beginFrame() = 0;
        virtual void endFrame() = 0;
        virtual RenderTargetRef createWindowTarget(Extent extent, void* windowHandle) const = 0;
        virtual CommandPoolRef createCommandPool() = 0;
        virtual TextureRef createTexture(Format format, Extent extent) = 0;
        virtual PipelineStateRef createPSO(const PipelineStateDesc& desc) = 0;

        virtual IndexBufferRef createIndexBuffer(const void* data, uint32_t size, IndexType type) = 0;
        virtual VertexBufferRef createVertexBuffer(const void* data, uint32_t size, BufferUsage usage) = 0;
        virtual UniformBufferRef createUniformBuffer(uint32_t size) = 0;

        virtual DeviceRef getDevice() {return mCurrentDevice;}

        NO_COPY_MOVE(RenderBackend);
    };
NS_END
