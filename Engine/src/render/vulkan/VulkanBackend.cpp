//
// Created by deril on 3/1/26.
//

#include "VoxEngine/render/vulkan/VulkanBackend.h"
#include "VoxEngine/render/vulkan/Debug.h"
#include <vulkan/vulkan.h>

VULKAN_NS
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
        appInfo.apiVersion = mApiVersion;

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

    }
NS_END

