#pragma once

#include "CommandPool.h"
#include "Fence.h"
#include "FrameSync.h"
#include "GraphicsPipeline.h"
#include "PhysicalDevice.h"
#include "Queue.h"
#include "Semaphore.h"
#include <vk_mem_alloc.h>
#include "VulkanObject.h"

VULKAN_NS
    class LogicalDevice : public VulkanObject<VkDevice> {
        VmaAllocator mAllocator;

        PhysicalDevice mPhysicalDevice;
        std::unordered_map<QueueType, Queue> mQueues;
        std::unordered_map<QueueType, CommandPool&> mCmdPools;

        LogicalDevice(VkDevice handle, const PhysicalDevice &physicalDevice, std::unordered_map<QueueType, Queue> queues);
    public:

        PhysicalDevice getPhysicalDevice() const;
        const std::unordered_map<QueueType, Queue>& getQueues() const;
        const std::unordered_map<QueueType, CommandPool&>& getCmdPools() const;

        const Queue &getQueue(QueueType type) const;
        CommandPool& getCmdPool(QueueType type) const;
        VmaAllocator getAllocator() const;

        void waitIdle() const;

        static LogicalDevice* Create(const PhysicalDevice &physDevice, std::vector<const char *> extensions, std::vector<const char *> validationLayers);
//
//        RenderPass createRenderPass(VkFormat format, RenderPassType type) const;
//        RenderPass *createRenderPassHeap(VkFormat format, RenderPassType type) const;
//        GraphicsPipeline createGraphicsPipeline(const RenderPass &pass, const std::vector<uint32_t> &vertexShader, const std::vector<uint32_t> &fragmentShader) const;
//        GraphicsPipeline *createGraphicsPipelineHeap(const RenderPass &pass, const std::vector<uint32_t> &vertexShader, const std::vector<uint32_t> &fragmentShader) const;
//        CommandPool createCommandPool(const QueueFamily &family) const;
//        CommandPool* createCommandPoolHeap(const QueueFamily &family) const;
//        Semaphore createSemaphore() const;
//        Semaphore* createSemaphoreHeap() const;
//        Fence createFence() const;
//        Fence* createFenceHeap() const;
//        FrameSync createFrame() const;
//        FrameSync* createFrameHeap() const;

        template<typename T, typename... Args>
        T create(Args&&... args) const {
            return T::Create(*this, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        T* createHeap(Args&&... args) const {
            return new T(T::Create(*this, std::forward<Args>(args)...));
        }
    };
NS_END