#include <stdexcept>
#include <vector>

#include <VoxEngine/render/shaders/ShaderRepository.h>
#include <VoxEngine/render/vulkan/Debug.h>
#include <VoxEngine/render/vulkan/LogicalDevice.h>
#include <VoxEngine/render/vulkan/Vertex.h>
#include <VoxEngine/render/vulkan/VulkanState.h>
#include <VoxEngine/render/windowing/Window.h>
#include <vulkan/vulkan_core.h>
#include "VoxEngine/render/vulkan/buffers/VertexBuffer.h"
#include "VoxEngine/render/vulkan/StagingBuffer.h"
#include <vk_mem_alloc.h>

namespace Vox::Render::Vulkan {
    const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME
    };

    VulkanState::VulkanState(int apiVersion, const Shader::ShaderRepository &repository) : mApiVersion(apiVersion), mShaderRepository(repository) {
    }

    void VulkanState::init() {
        createInstance();
        setupDebugMessenger(instance, debugMessenger);
        physicalDevices = PhysicalDevice::pickDevices(instance);
        physicalDevice = &physicalDevices[0];

        device = LogicalDevice::Create(*physicalDevice, deviceExtensions, VALIDATION_LAYERS);


        // if (!physicalDevice->isSwapChainAdequate()) {
        // throw std::runtime_error("Swap chain is not adequate!");
        // }
    }



    void VulkanState::createInstance() {
        if (ENABLE_VALIDATION_LAYERS && !checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "N";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = mApiVersion;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (ENABLE_VALIDATION_LAYERS) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
            createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance), "Failed to create instance!");
    }

    VmaAllocator VulkanState::createAllocator(const LogicalDevice &device) const {
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        allocatorCreateInfo.physicalDevice = device.getPhysicalDevice().getHandle();
        allocatorCreateInfo.device = device.getHandle();
        allocatorCreateInfo.instance = instance;
        allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

        VmaAllocator allocator;
        vmaCreateAllocator(&allocatorCreateInfo, &allocator);
        return allocator;
    }

//    const std::vector<Vertex> vertices = {
//            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
//            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
//            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
//    };
//
//    const std::vector<uint16_t> indices = {
//            0, 1, 2, 2, 3, 0
//    };

    IndexBuffer* VulkanState::createIndexBuffer(std::vector<uint32_t> indices) {
        auto staging = device->create<StagingBuffer>(2 * MEGABYTE_SIZE);

        const Queue& transfer = device->getQueue(TRANSFER_QUEUE);
        CommandPool& pool = device->getCmdPool(TRANSFER_QUEUE);

        size_t size1 = sizeof(indices[0]) * indices.size();
        IndexBuffer* buffer = device->createHeap<IndexBuffer>(size1, VK_INDEX_TYPE_UINT32);

        VkCommandBuffer cmdBuffer = pool.startTemp();
        staging.write((void *) indices.data(), size1);
        staging.copy(cmdBuffer, *buffer ,size1);
        pool.submitTemp(transfer);
        return buffer;
    }


    VertexBuffer* VulkanState::createVertexBuffer(std::vector<Vertex> vertices) {
        auto staging = device->create<StagingBuffer>(4 * MEGABYTE_SIZE);

        const Queue& transfer = device->getQueue(TRANSFER_QUEUE);
        CommandPool& pool = device->getCmdPool(TRANSFER_QUEUE);

        size_t size1 = sizeof(vertices[0]) * vertices.size();
        VertexBuffer* buffer = device->createHeap<VertexBuffer>(size1);

        VkCommandBuffer cmdBuffer = pool.startTemp();
        staging.write((void *) vertices.data(), size1);
        staging.copy(cmdBuffer, *buffer ,size1);
        pool.submitTemp(transfer);
        return buffer;
    }

    static int i = 0;


    void VulkanState::cleanup() {
        // vkDestroySemaphore(logicalDevice->getHandle(), renderFinishedSemaphore, nullptr);
        // vkDestroySemaphore(logicalDevice->getHandle(), imageAvailableSemaphore, nullptr);
        // vkDestroyFence(logicalDevice->getHandle(), inFlightFence, nullptr);
        //
        // vkDestroyCommandPool(device, commandPool, nullptr);
        //
        // for (auto framebuffer : swapChainFramebuffers) {
        //     vkDestroyFramebuffer(device, framebuffer, nullptr);
        // }
        //
        // vkDestroyPipeline(device, graphicsPipeline, nullptr);
        // vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        // vkDestroyRenderPass(device, renderPass, nullptr);
        //
        // for (auto imageView : swapChainImageViews) {
        //     vkDestroyImageView(device, imageView, nullptr);
        // }
        //
        // vkDestroySwapchainKHR(device, swapChain, nullptr);
        // vkDestroyDevice(device, nullptr);
        //
        // if (ENABLE_VALIDATION_LAYERS) {
        //     DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        // }
        //
        // vkDestroySurfaceKHR(instance, surface, nullptr);
        // vkDestroyInstance(instance, nullptr);
    }

    VulkanState* VulkanState::sInstance = nullptr;

    VulkanState *VulkanState::Get() {
        VOX_ASSERT(sInstance != nullptr, "Vulkan not initialized yet");
        return sInstance;
    }

    void VulkanState::initialize(int apiVersion, const Shader::ShaderRepository &shaders) {
        if (sInstance != nullptr) return;
        sInstance = new VulkanState(apiVersion, shaders);
        sInstance->init();
    }
}
