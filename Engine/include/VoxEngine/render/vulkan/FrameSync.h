#pragma once

#include <optional>
#include <vulkan/vulkan_core.h>

#include "Fence.h"
#include "Queue.h"
#include "Semaphore.h"
#include "SwapChain.h"
#include "VulkanCommandBuffer.h"

VULKAN_NS
    class VulkanDevice;

    class FrameSync {
        friend class VulkanDevice;

        const VulkanDevice &mDevice;
        Fence mFence;
        Semaphore mImageWaitSemaphore;
        CommandBufferRef mCommandBuffer;

        bool mStarted = false;
        uint32_t mCurrentImageIndex = 0;

        std::optional<Semaphore> mRenderWaitSemaphore;

        FrameSync(const VulkanDevice &device, Fence fence, Semaphore imageSemaphore, CommandBufferRef buffer);

        static FrameSync Create(const VulkanDevice &device);

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
