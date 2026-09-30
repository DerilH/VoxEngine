//
// Created by deril on 2/13/26.
//

#pragma once
#include <vulkan/vulkan_core.h>

#include "VoxEngine/render/passes/RenderPass.h"
#include "VoxEngine/render/vulkan/VulkanObject.h"
#include "Semaphore.h"
#include "VulkanTexture.h"

VULKAN_NS
    class VulkanDevice;
    class Surface;

    class SwapChain : public VulkanObject<VkSwapchainKHR> {
        friend class Surface;
        friend class VulkanDevice;

        Vector<VulkanTexture*> mTextures;
        const VulkanDevice& mDevice;
        Extent mExtent;
        bool mNeedsRebuild = false;

        SwapChain(const VulkanDevice &device, VkSwapchainKHR mHandle, Vector<VulkanTexture*>& textures, Extent extent);

        static SwapChain* Create(const Surface &surface, VkSwapchainKHR old = nullptr);
    public:
        VkResult acquireNextImage(const Semaphore &semaphore, uint32_t *imageIndex) const;

        ~SwapChain();

        VulkanTexture* getTexture(uint32_t index) const;

        Extent getExtent() const;
        int getImageCount() const;
        bool needsRebuild() const;
        void markForRebuild();

        static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes);
        static Extent chooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities, const int width, const int height);

        NO_COPY_MOVE_DEFAULT(SwapChain)
    };
NS_END