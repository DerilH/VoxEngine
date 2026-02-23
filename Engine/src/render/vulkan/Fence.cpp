//
// Created by deril on 2/16/26.
//


#include <VoxEngine/render/vulkan/Fence.h>
#include <VoxEngine/render/vulkan/LogicalDevice.h>

VULKAN_NS
    Fence::Fence(const VkFence fence) : mHandle(fence) {
    }

    Fence Fence::Create(const LogicalDevice &device) {
        VkFenceCreateInfo fenceInfo{};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VkFence fence;
        VK_CHECK(vkCreateFence(device.getHandle(), &fenceInfo, nullptr, &fence), "Cannot create fence");
        return Fence(fence);
    }

    VkFence Fence::getHandle() const {
        return mHandle;
    }

NS_END
