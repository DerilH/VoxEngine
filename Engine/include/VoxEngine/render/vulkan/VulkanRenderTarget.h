//
// Created by deril on 2/17/26.
//

#pragma once
#include <VoxEngine/render/RenderPassType.h>
#include <VoxEngine/render/RenderTarget.h>

#include "FrameSync.h"

VULKAN_NS
    class VulkanRenderTarget : public RenderTarget {
    protected:
        const LogicalDevice *mDevice;
        std::vector<Semaphore> mRenderFinishedSemaphores;
        std::vector<FrameSync> mFrames;
        int mCurrentFrame = 0;
        VkExtent2D mExtent;
        std::unordered_map<RenderPassType, RenderPass*> mRenderPasses;

        void createFrames(char buffers);

        void nextFrame();

        explicit VulkanRenderTarget(const LogicalDevice *device, VkExtent2D extent);

        ~VulkanRenderTarget() = default;

    public:
        virtual bool begin() = 0;

        virtual void present() = 0;

        void submit();

        void setBuffering(char buffers);

        const FrameSync &getCurrentFrame() const;

        VkExtent2D getExtent() const;

        virtual RenderPass& getRenderPass(RenderPassType type) const = 0;

        virtual VkFramebuffer getFramebuffer(RenderPassType type) const = 0;

        virtual void addRenderPass(RenderPassType type) = 0;

        virtual VkFormat getFormat() const = 0;
    };

NS_END
