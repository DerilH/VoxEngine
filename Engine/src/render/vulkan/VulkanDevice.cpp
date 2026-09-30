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

    VulkanDevice::VulkanDevice(const VkDevice handle, const PhysicalDevice &physicalDevice, std::unordered_map<QueueType, Queue> queues) : VulkanObject(handle),
                                                                                                                                           mPhysicalDevice(physicalDevice),
                                                                                                                                           mQueues(std::move(queues)) {
        for (const auto [type, queue]: mQueues) {
            mCmdPools.emplace(type, *this->createHeap<VulkanCommandPool>(queue.getFamily()));
        }
        mAllocator = VulkanBackend::Get()->createAllocator(*this);

        initGlobalDescriptors();
    }

    void VulkanDevice::initGlobalDescriptors() {
        mGlobalDescriptorPool = this->createHeap<VulkanDescriptorPool>(100, ArrayView<VkDescriptorPoolSize>({{toVk(ShaderResourceType::UNIFORM_BUFFER), 100}}));

        // Layout для set = 0 (GlobalData)
        {
            VkDescriptorSetLayoutCreateInfo createInfo{};
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

        // Layout для set = 1 (ModelData)
        {
            VkDescriptorSetLayoutCreateInfo createInfo{};
            createInfo.bindingCount = 1;
            VkDescriptorSetLayoutBinding binding{};
            binding.descriptorCount = 1;
            binding.binding = 0;
            binding.descriptorType = toVk(ShaderResourceType::UNIFORM_BUFFER);
            binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            createInfo.pBindings = &binding;

            vkCreateDescriptorSetLayout(*this, &createInfo, nullptr, &mModelDescriptorSetLayout);
        }

        // Инициализация Uniform Buffer с 2-мя identity матрицами (view, proj)
        struct GlobalData {
            glm::mat4 view;
            glm::mat4 proj;
        };

        mGlobalUniformBuffer = this->createHeap<VulkanUniformBuffer>(sizeof(GlobalData));

        GlobalData data{};

        // 1. Матрица вида (Камера находится в точке (0, 0, 3) и смотрит в центр (0, 0, 0))
        glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraUp     = glm::vec3(0.0f, 1.0f, 0.0f);

        data.view = glm::lookAt(cameraPos, cameraTarget, cameraUp);

        // 2. Матрица перспективной проекции (FOV 45 градусов, соотношение сторон 16:9)
        float fov    = glm::radians(45.0f);
        float aspect = 16.0f / 9.0f; // В реальном коде подставляйте actualWidth / actualHeight из Swapchain
        float zNear  = 0.1f;
        float zFar   = 100.0f;

        data.proj = glm::perspective(fov, aspect, zNear, zFar);

        // ВНИМАНИЕ: Инвертируем ось Y для Vulkan (GLM рассчитан на OpenGL, где Y направлен вверх)
        data.proj[1][1] *= -1.0f;
        void* mappedData = mGlobalUniformBuffer->getAllocationInfo().pMappedData;
        if (mappedData) {
            memcpy(mappedData, &data, sizeof(GlobalData));
        }

        // Подключение буфера к дескриптор сету
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

        HashMap<QueueType, Queue> queues;
        for (const auto &queueFamily: queueFamilies.getUniqueFamilies()) {
            VkQueue queue = acquireQueue(device, queueFamilies, queueFamily.type());
            queues.emplace(queueFamily.type(), Queue{queue, queueFamily});
        }

        for (const auto &queueFamily: queueFamilies.getUniqueFamilies()) {
            VkQueue queue = acquireQueue(device, queueFamilies, queueFamily.type());
            queues.emplace(queueFamily.type(), Queue{queue, queueFamily});
        }

        return new VulkanDevice{device, physDevice, std::move(queues)};
    }

    VkBuffer VulkanDevice::allocateBuffer(VkBufferCreateInfo &bufferCreateInfo, const VmaAllocationCreateInfo &allocInfo, const bool exclusive, VmaAllocation &allocation, VmaAllocationInfo &info) const {
        bufferCreateInfo.sharingMode = exclusive ? VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT;

        VkBuffer buffer = VK_NULL_HANDLE;
        VK_CHECK(vmaCreateBuffer(getAllocator(), &bufferCreateInfo, &allocInfo, &buffer, &allocation, &info), "failed to allocate buffers");
        return buffer;
    }
}
