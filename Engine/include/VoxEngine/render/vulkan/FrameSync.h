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

        const LogicalDevice& mDevice;
        Fence mFence;
        Semaphore mImageWaitSemaphore;
        CommandBuffer mCommandBuffer ;

        bool mStarted = false;
        uint32_t mCurrentImageIndex = 0;

        std::optional<Semaphore> mRenderWaitSemaphore;

        FrameSync(const LogicalDevice& device, Fence fence, Semaphore imageSemaphore, CommandBuffer buffer);
        static FrameSync Create(const LogicalDevice &device);
    public:
        uint32_t begin(std::optional<std::reference_wrapper<SwapChain>> swapChain);

        void submit();

        const Semaphore& getRenderWaitSemaphore() const;

        const Semaphore &getImageWaitSemaphore() const;

        CommandBuffer getCmdBuffer() const;

        uint32_t getCurrentImageIndex() const;
        void setRenderWaitSemaphore(const Semaphore& semaphore);
    };

NS_END
