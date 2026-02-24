#pragma once

#include <optional>
#include <vulkan/vulkan_core.h>

#include "Fence.h"
#include "Queue.h"
#include "Semaphore.h"
#include "SwapChain.h"
#include "CommandBuffer.h"

VULKAN_NS
    class LogicalDevice;

    class FrameSync {
        friend class LogicalDevice;

        const LogicalDevice &mDevice;
        Fence mFence;
        Semaphore mImageWaitSemaphore;
        CommandBuffer mCommandBuffer;

        bool mStarted = false;
        uint32_t mCurrentImageIndex = 0;

        std::optional<Semaphore> mRenderWaitSemaphore;

        FrameSync(const LogicalDevice &device, Fence fence, Semaphore imageSemaphore, CommandBuffer buffer);

        static FrameSync Create(const LogicalDevice &device);

    public:
        uint32_t begin(std::optional<std::reference_wrapper<SwapChain>> swapChain);

        void submit();

        const Semaphore &getRenderWaitSemaphore() const { return mRenderWaitSemaphore.value(); }
        const Semaphore &getImageWaitSemaphore() const { return mImageWaitSemaphore; }
        CommandBuffer getCmdBuffer() const { return mCommandBuffer; }
        uint32_t getCurrentImageIndex() const { return mCurrentImageIndex; }
        void setRenderWaitSemaphore(const Semaphore &semaphore) { mRenderWaitSemaphore.emplace(semaphore);}
    };

NS_END
