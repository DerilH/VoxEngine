#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include <VoxEngine/render/vulkan/VulkanState.h>
#include "VoxEngine/render/vulkan/VulkanBackend.h"

namespace Vox::Render::Vulkan {
    VkQueue acquireQueue(const VkDevice device, const QueueFamilyRepository &queueFamilies, const QueueType queueType,
                         const int index = 0) {
        VkQueue queue{};
        vkGetDeviceQueue(device, queueFamilies[queueType].index(), index, &queue);
        return queue;
    }

    VulkanDevice::VulkanDevice(const VkDevice handle, const PhysicalDevice &physicalDevice, std::unordered_map<QueueType, Queue> queues) : VulkanObject(handle),
                                                                                                                                           mPhysicalDevice(physicalDevice),
                                                                                                                                           mQueues(std::move(queues)) {
        for (const auto [type, queue]: mQueues) {
            mCmdPools.emplace(type, *this->createHeap<VulkanCommandPool>(queue.getFamily()));
        }
        mAllocator = VulkanBackend::Get()->createAllocator(*this);
    }

    PhysicalDevice VulkanDevice::getPhysicalDevice() const {
        return mPhysicalDevice;
    }

    const std::unordered_map<QueueType, Queue> &VulkanDevice::getQueues() const {
        return mQueues;
    }

    const std::unordered_map<QueueType, VulkanCommandPool &> &VulkanDevice::getCmdPools() const {
        return mCmdPools;
    }

    const Queue &VulkanDevice::getQueue(const QueueType type) const {
        return mQueues.at(type);
    }

    VulkanCommandPool &VulkanDevice::getCmdPool(const QueueType type) const {
        return mCmdPools.at(type);
    }

    VmaAllocator VulkanDevice::getAllocator() const {
        return mAllocator;
    }

    void VulkanDevice::waitIdle() const {
        vkDeviceWaitIdle(mHandle);
    }

    VulkanDevice *VulkanDevice::Create(const PhysicalDevice &physDevice, std::vector<const char *> extensions,
                                       std::vector<const char *> validationLayers) {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        QueueFamilyRepository queueFamilies = physDevice.getQueueFamilies();

        float queuePriority = 1.0f;
        for (QueueFamily queueFamily: queueFamilies.getUniqueFamilies()) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily.index();
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
        }

        constexpr VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.pQueueCreateInfos = queueCreateInfos.data();

//        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkPhysicalDeviceDynamicRenderingFeatures dynamicRendering{};
        dynamicRendering.sType =
                VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
        dynamicRendering.dynamicRendering = VK_TRUE;


        VkPhysicalDeviceSynchronization2Features sync2{};
        sync2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES;
        sync2.synchronization2 = VK_TRUE;
        sync2.pNext = &dynamicRendering;

        VkPhysicalDeviceFeatures2 features2{};
        features2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
        features2.pNext = &sync2;

        createInfo.pNext = &features2;


#ifdef ENABLE_VALIDATION_LAYERS
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
#else
            createInfo.enabledLayerCount = 0;
#endif
        VkDevice device;
        VK_CHECK(vkCreateDevice(physDevice.getHandle(), &createInfo, nullptr, &device),
                 "failed to create logical device!");

        std::unordered_map<QueueType, Queue> queues;
        for (const auto &queueFamily: queueFamilies.getUniqueFamilies()) {
            VkQueue queue = acquireQueue(device, queueFamilies, queueFamily.type());
            queues.emplace(queueFamily.type(), Queue{queue, queueFamily});
        }

        std::unordered_map<QueueType, VulkanCommandPool> cmdPools;
        for (const auto &queueFamily: queueFamilies.getUniqueFamilies()) {
            VkQueue queue = acquireQueue(device, queueFamilies, queueFamily.type());
            queues.emplace(queueFamily.type(), Queue{queue, queueFamily});
        }

        return new VulkanDevice{device, physDevice, std::move(queues)};
    }
}
