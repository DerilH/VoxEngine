//
// Created by deril on 2/16/26.
//

#pragma once
#include <vulkan/vulkan_core.h>

VULKAN_NS
    class LogicalDevice;

    class Semaphore {
        friend class LogicalDevice;
        VkSemaphore mHandle = VK_NULL_HANDLE;

        explicit Semaphore(VkSemaphore semaphore);
        static Semaphore Create(const LogicalDevice& device);
public:
        VkSemaphore getHandle() const;
    };

NS_END
