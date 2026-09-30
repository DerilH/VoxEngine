//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/Assert.h"
#include "WindowRenderTarget.h"
#include "Device.h"
#include "Enums.h"
#include "VoxEngine/render/state/PipelineStateDesc.h"

RENDER_NS
    class RenderBackend {
        static RenderBackend* sInstance;
    protected:
        DeviceRef mCurrentDevice = nullptr;

        explicit RenderBackend() = default;
        virtual void init() = 0;
        ~RenderBackend() = default;

    public:

        virtual int32_t beginFrame() = 0;
        virtual void endFrame() = 0;
        virtual RenderTargetRef createWindowTarget(Extent extent, void* windowHandle) = 0;
        virtual CommandPoolRef createCommandPool() = 0;
        virtual TextureRef createTexture(Format format, Extent extent) = 0;
        virtual PipelineStateRef createPSO(const PipelineStateDesc& desc) = 0;

        virtual IndexBufferRef createIndexBuffer(const void* data, uint32_t size, IndexType type) = 0;
        virtual VertexBufferRef createVertexBuffer(const void* data, uint32_t size, BufferUsage usage) = 0;
        virtual UniformBufferRef createUniformBuffer(uint32_t size) = 0;

        virtual DeviceRef getDevice() {return mCurrentDevice;}

        static void Init(RenderBackend* backend) {
            VOX_ASSERT(!Initialized(), "Render backend already initialized");
            sInstance = backend;
            sInstance->init();
        }

        inline static RenderBackend* Get() {
            VOX_ASSERT(Initialized(), "Render backend not initialized");
            return sInstance;
        }

        inline static bool Initialized() {
            return sInstance != nullptr;
        }

        NO_COPY_MOVE(RenderBackend);
    };
NS_END
