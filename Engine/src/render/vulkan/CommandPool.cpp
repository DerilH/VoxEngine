//
// Created by deril on 2/14/26.
//


#include <VoxEngine/render/vulkan/Queue.h>
#include <VoxEngine/render/vulkan/CommandPool.h>
#include <VoxEngine/render/vulkan/LogicalDevice.h>

namespace Vox::Render::Vulkan {
    CommandPool::CommandPool(const VkCommandPool handle, const LogicalDevice& device) : mDevice(device), mHandle(handle) {
    }

    CommandPool CommandPool::Create(const LogicalDevice &device, const QueueFamily &family) {
        VkCommandPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = family.index();

        VkCommandPool commandPool;
        VK_CHECK(vkCreateCommandPool(device.getHandle(), &poolInfo, nullptr, &commandPool), "failed to create command pool!");
        return CommandPool(commandPool, device);
    }

    VkCommandPool CommandPool::getHandle() const {
        return mHandle;
    }

    CommandBuffer CommandPool::allocateBuffer() const {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = mHandle;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        VK_CHECK(vkAllocateCommandBuffers(mDevice.getHandle(), &allocInfo, &commandBuffer), "failed to allocate command buffers!");
        return CommandBuffer(commandBuffer);
    }

    const CommandBuffer& CommandPool::startTemp() {
        VOX_CHECK(mOneTimeBuffer == nullptr, "one time buffer already created");
        mOneTimeBuffer = new CommandBuffer(allocateBuffer());
        mOneTimeBuffer->begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);
        return *mOneTimeBuffer;
    }

    void CommandPool::submitTemp(const Queue &queue) {
        VOX_CHECK(mOneTimeBuffer != nullptr, "one time buffer not started");
        mOneTimeBuffer->end();
        queue.submit(*mOneTimeBuffer, true);
        VkCommandBuffer bfs = {mOneTimeBuffer->getHandle()};
        vkFreeCommandBuffers(mDevice.getHandle(), mHandle, 1, &bfs);
        mOneTimeBuffer = nullptr;
    }

}
