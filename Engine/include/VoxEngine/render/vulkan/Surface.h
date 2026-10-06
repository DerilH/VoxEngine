//
// Created by deril on 2/13/26.
//

#pragma once
#include <VoxEngine/render/windowing/Window.h>
#include <VoxEngine/render/vulkan/Surface.h>
#include <VoxEngine/render/vulkan/VulkanObject.h>
#include <vulkan/vulkan_core.h>
#include "VoxEngine/render/WindowRenderTarget.h"
#include "VoxCore/containers/Containers.h"
#include "VoxCore/render/Enums.h"
#include "VoxEngine/render/vulkan/Semaphore.h"

VULKAN_NS
    class VulkanDevice;
    class QueueFamily;
    class Queue;
    class VulkanFrameSync;
    class SwapChain;

    struct SwapChainSupportDetails {
        VkSurfaceCapabilitiesKHR capabilities() const {
            return mCapabilities;
        }

        Vector<VkSurfaceFormatKHR> formats() const {
            return mFormats;
        }

        Vector<VkPresentModeKHR> presentModes() const {
            return mPresentModes;
        }

    private:
        VkSurfaceCapabilitiesKHR mCapabilities{};
        Vector<VkSurfaceFormatKHR> mFormats;
        Vector<VkPresentModeKHR> mPresentModes;

        SwapChainSupportDetails() = default;

        SwapChainSupportDetails(const VkSurfaceCapabilitiesKHR & capabilities, const Vector<VkSurfaceFormatKHR> & formats, const Vector<VkPresentModeKHR> & presentModes);
        friend class Surface;
    };

    class Surface : public WindowRenderTarget, public VulkanObject<VkSurfaceKHR> {
        friend class VulkanDevice;
        friend class SwapChain;

        void* mWindow;
        VulkanDevice* mDevice = nullptr;
        const Queue* mPresentQueue = nullptr;
        SwapChain* mCurrentSwapChain = nullptr;
        VkSurfaceFormatKHR mSurfaceFormat;
        Format mImageFormat;
        Extent mExtent;

        Vector<VulkanFrameSync*> mFrames{};
        Vector<Semaphore*> mRenderFinishedSemaphores;
        uint8_t mCurrentFrame = 0;

        explicit Surface(Extent extent, VkSurfaceKHR handle, void* windowHandle);

        SwapChainSupportDetails querySwapChainSupport() const;
        std::optional<QueueFamily> findPresentFamily() const;
        void createSwapChain();
        void createFrames(uint8_t buffers);

    public:
        Surface() = delete;

        static Surface *Create(Extent extent, VkInstance instance, void* windowHandle);

        int32_t beginFrame() override;
        void endFrame() override;
        void update();

        SwapChain& getSwapChain() const;
        Format getImageFormat() const;
        VkSurfaceFormatKHR getSurfaceFormat() const;

        void setDevice(VulkanDevice* device);
        const VulkanDevice* getDevice() const;
        const Queue *getPresentQueue() const;
        void presentFrame(const VulkanFrameSync &frame) const;

        void resize(Extent extent) override;

        TextureRef getBackBuffer() override;
        VulkanFrameSync& getCurrentFrame() const;
    };
NS_END