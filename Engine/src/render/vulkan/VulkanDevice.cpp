#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <VoxEngine/render/vulkan/VulkanDevice.h>
#include <VoxEngine/render/vulkan/VulkanState.h>
#include <VoxEngine/render/vulkan/VulkanUtil.h>
#include <VoxEngine/render/vulkan/VulkanDescriptorSet.h>
#include <VoxEngine/render/vulkan/buffers/VulkanUniformBuffer.h>

#include "VoxEngine/render/vulkan/VulkanBackend.h"

namespace Vox::Render::Vulkan {
    VkQueue acquireQueue(const VkDevice device, const QueueFamilyRepository &queueFamilies, const QueueType queueType,
                         const int index = 0) {
        VkQueue queue{};
        vkGetDeviceQueue(device, queueFamilies[queueType].index(), index, &queue);
        return queue;
    }

    VulkanDevice::VulkanDevice(const VkDevice handle, const PhysicalDevice &physicalDevice, std::unordered_map<QueueType, Queue> queues, VkInstance vkInstance) : VulkanObject(handle),
                                                                                                                                           mPhysicalDevice(physicalDevice),
                                                                                                                                           mQueues(std::move(queues)), mVkInstance(vkInstance) {
        for (const auto [type, queue]: mQueues) {
            mCmdPools.emplace(type, *this->createHeap<VulkanCommandPool>(queue.getFamily()));
        }
        mAllocator = createAllocator();
        initGlobalDescriptors();
    }

    void VulkanDevice::initGlobalDescriptors() {
        mGlobalDescriptorPool = this->createHeap<VulkanDescriptorPool>(100, ArrayView<VkDescriptorPoolSize>({{toVk(ShaderResourceType::UNIFORM_BUFFER), 100}}));

        {
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount = 1;
            VkDescriptorSetLayoutBinding binding{};
            binding.descriptorCount = 1;
            binding.descriptorType = toVk(ShaderResourceType::UNIFORM_BUFFER);
            binding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;
            createInfo.pBindings = &binding;

            VkDescriptorSetLayout layout;
            vkCreateDescriptorSetLayout(*this, &createInfo, nullptr, &layout);
            mGlobalDescriptors = this->createHeap<VulkanDescriptorSet>(mGlobalDescriptorPool, layout);
        }

        {
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
            createInfo.bindingCount = 1;
            VkDescriptorSetLayoutBinding binding{};
            binding.descriptorCount = 1;
            binding.binding = 0;
            binding.descriptorType = toVk(ShaderResourceType::UNIFORM_BUFFER);
            binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            createInfo.pBindings = &binding;

            vkCreateDescriptorSetLayout(*this, &createInfo, nullptr, &mModelDescriptorSetLayout);
        }

        struct GlobalData {
            glm::mat4 view;
            glm::mat4 proj;
        };

        mGlobalUniformBuffer = this->createHeap<VulkanUniformBuffer>(sizeof(GlobalData));

        GlobalData data{};

        glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp     = glm::vec3(0.0f, 1.0f, 0.0f);

        data.view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        float fov    = glm::radians(45.0f);
        float aspect = 1400.0f / 900.0f;
        float zNear  = 0.1f;
        float zFar   = 100.0f;

        data.proj = glm::perspective(fov, aspect, zNear, zFar);

        data.proj[1][1] *= -1.0f;
        void* mappedData = mGlobalUniformBuffer->getAllocationInfo().pMappedData;
        if (mappedData) {
            memcpy(mappedData, &data, sizeof(GlobalData));
        }

        mGlobalDescriptors->update(*this, 0, *mGlobalUniformBuffer);
    }

    PhysicalDevice VulkanDevice::getPhysicalDevice() const {
        return mPhysicalDevice;
    }

    const HashMap<QueueType, Queue> &VulkanDevice::getQueues() const {
        return mQueues;
    }

    const HashMap<QueueType, VulkanCommandPool &> &VulkanDevice::getCmdPools() const {
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

    const VulkanDescriptorSetRef &VulkanDevice::getGlobalDescriptorSet() const {
        return mGlobalDescriptors;
    }

    VkDescriptorSetLayout VulkanDevice::getModelDescriptorSetLayout() const {
        return mModelDescriptorSetLayout;
    }

    VulkanDescriptorSetRef VulkanDevice::createDescriptorSet(VkDescriptorSetLayout layout) const {
        return this->createHeap<VulkanDescriptorSet>(mGlobalDescriptorPool, layout);
    }

    void VulkanDevice::waitIdle() const {
        vkDeviceWaitIdle(mHandle);
    }

    VulkanDevice *VulkanDevice::Create(VkInstance vkInstance, const PhysicalDevice &physDevice,
                                       std::vector<const char *> extensions, std::vector<const char *> validationLayers) {
        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::vector<int> usedFamilies;
        QueueFamilyRepository queueFamilies = physDevice.getQueueFamilies();

        float queuePriority = 1.0f;
        for (QueueFamily queueFamily: queueFamilies.getUniqueFamilies()) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily.index();
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            if (std::ranges::find(usedFamilies, queueFamily.index()) == usedFamilies.end()) {
                queueCreateInfos.push_back(queueCreateInfo);
                usedFamilies.push_back(queueFamily.index());
            }
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
        features2.features.fillModeNonSolid = VK_TRUE;
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

        HashMap<QueueType, Queue> queues;
        for (const auto &queueFamily: queueFamilies.getUniqueFamilies()) {
            VkQueue queue = acquireQueue(device, queueFamilies, queueFamily.type());
            queues.emplace(queueFamily.type(), Queue{queue, queueFamily});
        }

        return new VulkanDevice{device, physDevice, std::move(queues), vkInstance};
    }

    VkBuffer VulkanDevice::allocateBuffer(VkBufferCreateInfo &bufferCreateInfo, const VmaAllocationCreateInfo &allocInfo, const bool exclusive, VmaAllocation &allocation, VmaAllocationInfo &info) const {
        bufferCreateInfo.sharingMode = exclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

        VkBuffer buffer = VK_NULL_HANDLE;
        VK_CHECK(vmaCreateBuffer(getAllocator(), &bufferCreateInfo, &allocInfo, &buffer, &allocation, &info), "failed to allocate buffers");
        return buffer;
    }

    VmaAllocator VulkanDevice::createAllocator() {
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        allocatorCreateInfo.physicalDevice = mPhysicalDevice.getHandle();
        allocatorCreateInfo.device = mHandle;
        allocatorCreateInfo.instance = mVkInstance;
        allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

        VmaAllocator allocator;
        vmaCreateAllocator(&allocatorCreateInfo, &allocator);
        return allocator;
    }
}
