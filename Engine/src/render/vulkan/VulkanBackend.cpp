//
// Created by deril on 3/1/26.
//

#include "VoxEngine/render/vulkan/VulkanBackend.h"
#include "VoxEngine/render/vulkan/Debug.h"
#include "VoxEngine/render/vulkan/VulkanDevice.h"
#include <vulkan/vulkan.h>

VULKAN_NS
    const std::vector<const char *> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        "VK_KHR_synchronization2"
    };

    void VulkanBackend::createInstance() {
#ifdef VK_ENABLE_VALIDATION
        if (!checkValidationLayerSupport()) {
            throw std::runtime_error("validation layers requested, but not available!");
        }
#endif

        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "N";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        //TODO: Add version choice
        appInfo.apiVersion = VK_API_VERSION_1_4;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};

#ifdef VK_ENABLE_VALIDATION_LAYERS
            createInfo.enabledLayerCount = static_cast<uint32_t>(VALIDATION_LAYERS.size());
            createInfo.ppEnabledLayerNames = VALIDATION_LAYERS.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *) &debugCreateInfo;
#else
            createInfo.enabledLayerCount = 0;
            createInfo.pNext = nullptr;
#endif

        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &mInstance), "Failed to create vulkan instance!");
    }

    void VulkanBackend::init() {
        createInstance();

        setupDebugMessenger(mInstance, mDebugMessenger);
        auto physicalDevices = PhysicalDevice::pickDevices(mInstance);
        mCurrentDevice = VulkanDevice::Create(physicalDevices[0], deviceExtensions, VALIDATION_LAYERS);
    }

    void VulkanBackend::beginFrame() {

    }

    void VulkanBackend::endFrame() {

    }

    RenderTargetRef VulkanBackend::createWindowTarget(Extent extent, void* windowHandle) {
        return nullptr;
    }

    VmaAllocator VulkanBackend::createAllocator(const VulkanDevice& device) {
        VmaVulkanFunctions vulkanFunctions = {};
        vulkanFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
        vulkanFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT;
        allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_2;
        allocatorCreateInfo.physicalDevice = device.getPhysicalDevice().getHandle();
        allocatorCreateInfo.device = device.getHandle();
        allocatorCreateInfo.instance = mInstance;
        allocatorCreateInfo.pVulkanFunctions = &vulkanFunctions;

        VmaAllocator allocator;
        vmaCreateAllocator(&allocatorCreateInfo, &allocator);
        return allocator;
    }


NS_END

