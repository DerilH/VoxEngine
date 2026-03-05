#pragma once

#include "VulkanCommandPool.h"
#include "Fence.h"
#include "FrameSync.h"
#include "VoxEngine/render/vulkan/pipeline/GraphicsPipeline.h"
#include "PhysicalDevice.h"
#include "Queue.h"
#include "Semaphore.h"
#include <vk_mem_alloc.h>
#include "VulkanObject.h"
#include "VoxCore/math/Extent.h"
#include "VoxEngine/render/Device.h"

VULKAN_NS
    class VulkanDevice : public Device, public VulkanObject<VkDevice> {
        VmaAllocator mAllocator;

        PhysicalDevice mPhysicalDevice;
        std::unordered_map<QueueType, Queue> mQueues;
        std::unordered_map<QueueType, VulkanCommandPool&> mCmdPools;

        VulkanDevice(VkDevice handle, const PhysicalDevice &physicalDevice, std::unordered_map<QueueType, Queue> queues);
    public:

        PhysicalDevice getPhysicalDevice() const;
        const std::unordered_map<QueueType, Queue>& getQueues() const;
        const std::unordered_map<QueueType, VulkanCommandPool&>& getCmdPools() const;

        const Queue &getQueue(QueueType type) const;
        VulkanCommandPool& getCmdPool(QueueType type) const;
        VmaAllocator getAllocator() const;

        void waitIdle() const;

        static VulkanDevice* Create(const PhysicalDevice &physDevice, std::vector<const char *> extensions, std::vector<const char *> validationLayers);

        template<typename T, typename... Args>
        T create(Args&&... args) const {
            return T::Create(*this, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        T* createHeap(Args&&... args) const {
            return new T(T::Create(*this, std::forward<Args>(args)...));
        }

        template<typename T, typename... Args>
        auto builder(Args&&... args) const {
            return T::Builder(*this, std::forward<Args>(args)...);
        }
    };
NS_END