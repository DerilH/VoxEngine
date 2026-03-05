//
// Created by deril on 2/16/26.
//


#include <VoxEngine/render/vulkan/Fence.h>
#include <VoxEngine/render/vulkan/VulkanDevice.h>

VULKAN_NS
    Fence::Fence(const VkFence fence) : VulkanObject(fence) {
    }

    Fence Fence::Create(const VulkanDevice &device) {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkFence fence;
        VK_CHECK(vkCreateFence(device.getHandle(), &fenceInfo, nullptr, &fence), "Cannot create fence");
        return Fence(fence);
    }

    void Fence::wait(const VulkanDevice &device, const ArrayView<VkFence> &&wait, VkBool32 waitCount, uint64_t timeout) {
        vkWaitForFences(device, wait.size(), wait.pData, waitCount, timeout);
    }

NS_END
