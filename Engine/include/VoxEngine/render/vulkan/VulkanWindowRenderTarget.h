//
// Created by deril on 2/17/26.
//

#pragma once
#include "VulkanRenderTarget.h"

VULKAN_NS
    class VulkanWindowRenderTarget : protected VulkanRenderTarget {
        Surface &mSurface;

        ~VulkanWindowRenderTarget() = default;

    public:
        explicit VulkanWindowRenderTarget(Surface &surface);

        bool begin() override;

        void present() override;
    };

NS_END
