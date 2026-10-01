#pragma once

#include "VulkanCommandPool.h"
#include "VoxEngine/render/vulkan/pipeline/GraphicsPipeline.h"
#include "PhysicalDevice.h"
#include "Queue.h"
#include <vk_mem_alloc.h>

#include "VulkanDescriptorPool.h"
#include "VulkanObject.h"
#include "VulkanTypes.h"
#include "VoxEngine/render/Device.h"

VULKAN_NS
    class VulkanBackend;

    class VulkanDevice : public Device, public VulkanObject<VkDevice> {
        VmaAllocator mAllocator;

        PhysicalDevice mPhysicalDevice;
        HashMap<QueueType, Queue> mQueues;
        HashMap<QueueType, VulkanCommandPool&> mCmdPools;
        VulkanDescriptorPoolRef mGlobalDescriptorPool = nullptr;
        VulkanDescriptorSetRef mGlobalDescriptors = nullptr;
        VkDescriptorSetLayout mModelDescriptorSetLayout = VK_NULL_HANDLE;
        class VulkanUniformBuffer* mGlobalUniformBuffer = nullptr;
        VkInstance mVkInstance;

        VulkanDevice(VkDevice handle, const PhysicalDevice &physicalDevice, HashMap<QueueType, Queue> queues, VkInstance backend);

        void initGlobalDescriptors();
        VmaAllocator createAllocator();
    public:

        PhysicalDevice getPhysicalDevice() const;
        const HashMap<QueueType, Queue>& getQueues() const;
        const HashMap<QueueType, VulkanCommandPool&>& getCmdPools() const;

        const Queue &getQueue(QueueType type) const;
        VulkanCommandPool& getCmdPool(QueueType type) const;
        VmaAllocator getAllocator() const;
        const VulkanDescriptorSetRef& getGlobalDescriptorSet() const;
        VkDescriptorSetLayout getModelDescriptorSetLayout() const;
        VulkanDescriptorSetRef createDescriptorSet(VkDescriptorSetLayout layout) const;

        void waitIdle() const;

        static VulkanDevice* Create(VkInstance vkInstance, const PhysicalDevice &physDevice, std::vector<const char *> extensions, std::vector<const char *> validationLayers);

        template<typename T, typename... Args>
        T create(Args &&... args) const {
            return T::Create(*this, std::forward<Args>(args)...);
        }

        template<typename T, typename... Args>
        T *createHeap(Args &&... args) const {
            return new T(T::Create(*this, std::forward<Args>(args)...));
        }

        template<typename T, typename... Args>
        auto builder(Args &&... args) const {
            return T::Builder(*this, std::forward<Args>(args)...);
        }

        VkBuffer allocateBuffer(VkBufferCreateInfo &bufferCreateInfo, const VmaAllocationCreateInfo &allocInfo, const bool exclusive, VmaAllocation &allocation, VmaAllocationInfo &info) const;
    };

NS_END
