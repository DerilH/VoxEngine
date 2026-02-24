//
// Created by deril on 2/16/26.
//

#pragma once
#include <vulkan/vulkan_core.h>

VULKAN_NS
    class LogicalDevice;

    class Semaphore : public VulkanObject<VkSemaphore>{
        friend class LogicalDevice;

        explicit Semaphore(VkSemaphore semaphore);
        static Semaphore Create(const LogicalDevice& device);
    };
NS_END
