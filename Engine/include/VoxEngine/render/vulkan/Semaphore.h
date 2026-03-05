//
// Created by deril on 2/16/26.
//

#pragma once
#include <vulkan/vulkan_core.h>

VULKAN_NS
    class VulkanDevice;

    class Semaphore : public VulkanObject<VkSemaphore>{
        friend class VulkanDevice;

        explicit Semaphore(VkSemaphore semaphore);
        static Semaphore Create(const VulkanDevice& device);
    };
NS_END
