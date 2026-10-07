#pragma once

#include <optional>

#include "Fence.h"
#include "Semaphore.h"
#include "SwapChain.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDescriptorPool.h"
#include "VulkanResourceCast.h"

VULKAN_NS
    class VulkanDevice;

    class VulkanFrameSync{
        friend class VulkanDevice;

        const VulkanDevice &mDevice;
        Fence mFence;
        Semaphore mImageWaitSemaphore;
        CommandBufferRef mCommandBuffer;
        VulkanDescriptorPoolRef mDescriptorPool = nullptr;

        bool mStarted = false;
        uint32_t mCurrentImageIndex = 0;

        std::optional<Semaphore> mRenderWaitSemaphore;

        VulkanFrameSync(const VulkanDevice &device, Fence fence, Semaphore imageSemaphore, CommandBufferRef buffer, VulkanDescriptorPoolRef pool);

        static VulkanFrameSync Create(const VulkanDevice &device);

    public:
        uint32_t begin(std::optional<std::reference_wrapper<SwapChain>> swapChain);

        void submit();

        const Semaphore &getRenderWaitSemaphore() const { return mRenderWaitSemaphore.value(); }
        const Semaphore &getImageWaitSemaphore() const { return mImageWaitSemaphore; }
        const CommandBufferRef getCmdBuffer() const { return mCommandBuffer; }
        uint32_t getCurrentImageIndex() const { return mCurrentImageIndex; }
        void setRenderWaitSemaphore(const Semaphore &semaphore) { mRenderWaitSemaphore.emplace(semaphore);}
    };

NS_END
