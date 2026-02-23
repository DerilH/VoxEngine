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

        VkFramebuffer getFramebuffer(RenderPassType type) const override;

        RenderPass &getRenderPass(RenderPassType type) const override;

        void addRenderPass(RenderPassType type) override;

        VkFormat getFormat() const override;
    };

NS_END
