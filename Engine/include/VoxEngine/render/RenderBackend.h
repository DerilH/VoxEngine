//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/Assert.h"
#include "WindowRenderTarget.h"
#include "Device.h"

RENDER_NS
    class RenderBackend {
        static RenderBackend* sInstance;
    protected:
        DeviceRef mCurrentDevice = nullptr;

        explicit RenderBackend() = default;
        virtual void init() = 0;
        ~RenderBackend() = default;

    public:

        virtual void beginFrame() = 0;
        virtual void endFrame() = 0;
        virtual RenderTargetRef createWindowTarget(Extent extent, void* windowHandle) = 0;
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
