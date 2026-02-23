//
// Created by deril on 2/16/26.
//

#include <VoxEngine/render/vulkan/LogicalDevice.h>
#include <VoxEngine/render/vulkan/Semaphore.h>

VULKAN_NS
    Semaphore::Semaphore(const VkSemaphore semaphore) : mHandle(semaphore) {
}

Semaphore Semaphore::Create(const LogicalDevice &device) {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkSemaphore semaphore;
    VK_CHECK(vkCreateSemaphore(device.getHandle(), &semaphoreInfo, nullptr, &semaphore), "Cannot create semaphore");
    return Semaphore(semaphore);
}

VkSemaphore Semaphore::getHandle() const {
    return mHandle;
}

NS_END
