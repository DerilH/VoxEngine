#include <VoxEngine/render/vulkan/LogicalDevice.h>
#include <VoxEngine/render/vulkan/VulkanState.h>

namespace Vox::Render::Vulkan {
    VkQueue acquireQueue(const VkDevice device, const QueueFamilyRepository &queueFamilies, const QueueType queueType,
                         const int index = 0) {
        VkQueue queue{};
        vkGetDeviceQueue(device, queueFamilies[queueType].index(), index, &queue);
        return queue;
    }

    LogicalDevice::LogicalDevice(const VkDevice handle, const PhysicalDevice &physicalDevice, std::unordered_map<QueueType, Queue> queues) : VulkanObject(handle),
                                                                                                                                             mPhysicalDevice(physicalDevice),
                                                                                                                                             mQueues(std::move(queues)) {
        for (const auto [type, queue]: mQueues) {
            mCmdPools.emplace(type, *this->createHeap<CommandPool>(queue.getFamily()));
        }
        mAllocator = VulkanState::Get()->createAllocator(*this);
    }

    PhysicalDevice LogicalDevice::getPhysicalDevice() const {
        return mPhysicalDevice;
    }

    const std::unordered_map<QueueType, Queue> &LogicalDevice::getQueues() const {
        return mQueues;
    }

    const std::unordered_map<QueueType, CommandPool &> &LogicalDevice::getCmdPools() const {
        return mCmdPools;
    }

    const Queue &LogicalDevice::getQueue(const QueueType type) const {
        return mQueues.at(type);
    }

    CommandPool &LogicalDevice::getCmdPool(const QueueType type) const {
        return mCmdPools.at(type);
    }

    VmaAllocator LogicalDevice::getAllocator() const {
        return mAllocator;
    }

    void LogicalDevice::waitIdle() const {
        vkDeviceWaitIdle(mHandle);
    }

    LogicalDevice *LogicalDevice::Create(const PhysicalDevice &physDevice, std::vector<const char *> extensions,
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

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkPhysicalDeviceDynamicRenderingFeatures dynamicRendering{};
        dynamicRendering.sType =
                VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
        dynamicRendering.dynamicRendering = VK_TRUE;

        createInfo.pNext = &dynamicRendering;


        if (ENABLE_VALIDATION_LAYERS) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
        } else {
            createInfo.enabledLayerCount = 0;
        }

        VkDevice device;
        VK_CHECK(vkCreateDevice(physDevice.getHandle(), &createInfo, nullptr, &device),
                 "failed to create logical device!");

        std::unordered_map<QueueType, Queue> queues;
        for (const auto &queueFamily: queueFamilies.getUniqueFamilies()) {
            VkQueue queue = acquireQueue(device, queueFamilies, queueFamily.type());
            queues.emplace(queueFamily.type(), Queue{queue, queueFamily});
        }

        std::unordered_map<QueueType, CommandPool> cmdPools;
        for (const auto &queueFamily: queueFamilies.getUniqueFamilies()) {
            VkQueue queue = acquireQueue(device, queueFamilies, queueFamily.type());
            queues.emplace(queueFamily.type(), Queue{queue, queueFamily});
        }

        return new LogicalDevice{device, physDevice, std::move(queues)};
    }
}
