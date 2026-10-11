#pragma once

#include <optional>
#include <VoxEngine/render/FrameSync.h>

#include "Fence.h"
#include "Semaphore.h"
#include "SwapChain.h"
#include "VulkanCommandBuffer.h"
#include "VulkanDescriptorPool.h"
#include "VulkanResourceCast.h"

VULKAN_NS
    class VulkanDevice;

    class SwapChainFrameSync : FrameSync{
        friend class VulkanDevice;

        const VulkanDevice &mDevice;
        Fence mFence;
        Semaphore mImageWaitSemaphore;
        CommandBufferRef mCommandBuffer;
        VulkanDescriptorPoolRef mDescriptorPool = nullptr;

        bool mStarted = false;
        uint32_t mCurrentImageIndex = 0;

        std::optional<Semaphore> mRenderWaitSemaphore;
        Ref<SwapChain> mSwapChain;

        SwapChainFrameSync(const VulkanDevice &device, Ref<SwapChain> swapChain, Fence fence, Semaphore imageSemaphore, CommandBufferRef buffer, VulkanDescriptorPoolRef pool);

        static SwapChainFrameSync Create(const VulkanDevice &device, Ref<SwapChain> swapChain);

    public:
        uint32_t begin() override;

        void submit() override;

        const Semaphore &getRenderWaitSemaphore() const { return mRenderWaitSemaphore.value(); }
        const Semaphore &getImageWaitSemaphore() const { return mImageWaitSemaphore; }
        Ref<CommandBuffer> getCmdBuffer() const override { return mCommandBuffer; }
        uint32_t getCurrentImageIndex() const { return mCurrentImageIndex; }
        void setRenderWaitSemaphore(const Semaphore &semaphore) { mRenderWaitSemaphore.emplace(semaphore);}
    };

NS_END
