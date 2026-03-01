//
// Created by deril on 2/15/26.
//

#pragma once
#include <cstring>
#include <vector>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include "VoxCore/Define.h"
#include "VoxCore/Assert.h"
#include "VoxCore/Severity.h"
#include "VoxCore/Logger.h"

namespace Vox::Render::Vulkan {
    const std::vector<const char*> VALIDATION_LAYERS = {
        "VK_LAYER_KHRONOS_validation"
    };

    inline VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
        if (const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")); func != nullptr) {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        } else {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    inline void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
        const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
            vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
        if (func != nullptr) {
            func(instance, debugMessenger, pAllocator);
        }
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
//        Log::log(std::format("[Validation Layer]: {}", pCallbackData->pMessage), Log::convertSeverity(messageSeverity));
        Log::log(std::string("[Validation Layer] ") + pCallbackData->pMessage, Log::convertSeverity(messageSeverity));
        return VK_FALSE;
    }

    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    inline void setupDebugMessenger(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger) {
        #ifndef VK_ENABLE_VALIDATION
            return;
        #endif

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        VK_CHECK(CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger), "failed to set up debug messenger!");
    }

    inline std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

#ifdef VK_ENABLE_VALIDATION
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

        extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
        extensions.emplace_back("VK_KHR_xlib_surface");


        return extensions;
    }

    inline bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : VALIDATION_LAYERS) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

}
