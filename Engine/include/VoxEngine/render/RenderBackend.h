//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/Pointers.h"
#include "Device.h"
#include "VoxCore/render/Enums.h"
#include "RenderCore.h"
#include "buffers/TransferBuffer.h"
#include "VoxEngine/render/state/PipelineStateDesc.h"
#include "VoxCore/math/Extent.h"

RENDER_NS
    class Window;

    class RenderBackend {
    protected:
        DeviceRef mCurrentDevice = nullptr;

        explicit RenderBackend(const RenderAPI api) : api(api) {}
        virtual ~RenderBackend() = default;

    public:
        explicit RenderBackend() = delete;

        const RenderAPI api;

        virtual void init() = 0;
        virtual int32_t beginFrame() = 0;
        virtual void endFrame() = 0;
        virtual RenderTargetRef createWindowTarget(Ref<Window> window) const = 0;
        virtual RenderTargetRef createTextureTarget(Extent extent, Format format, Format depthFormat = Format::D32) const = 0;
        virtual CommandPoolRef createCommandPool() = 0;
        virtual TextureRef createTexture(Format format, Extent extent) const = 0;
        virtual TextureRef createTexture(Format format, Extent extent, const void* data, uint32_t size) const = 0;

        virtual IndexBufferRef createIndexBuffer(const void* data, uint32_t size, IndexType type) = 0;
        virtual VertexBufferRef createVertexBuffer(const void* data, uint32_t size, BufferUsage usage) = 0;
        virtual Ref<TransferBuffer> createTransferBuffer(uint32_t size) = 0;
        virtual UniformBufferRef createUniformBuffer(uint32_t size) = 0;

        DeviceRef getDevice() const {return mCurrentDevice;}

        NO_COPY_MOVE(RenderBackend);
    };
NS_END
