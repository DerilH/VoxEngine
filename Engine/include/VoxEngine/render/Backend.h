//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/SingletonBase.h"
#include "Renderer.h"
#include "VoxCore/Assert.h"
#include "VoxEngine/render/vulkan/VulkanBackend.h"

RENDER_NS
    class Backend {
        static Backend* sInstance;
    protected:
        explicit Backend() = default;
        virtual void init() = 0;
        ~Backend() = default;

    public:

        static void Init(Backend* backend) {
            VOX_ASSERT(sInstance == nullptr, "Render backend already initialized");
            sInstance = backend;
            sInstance->init();
        }

        inline static Backend* Get() {
            VOX_ASSERT(sInstance != nullptr, "Render backend not initialized");
            return sInstance;
        }

        NO_COPY_MOVE(Backend);
    };
NS_END
