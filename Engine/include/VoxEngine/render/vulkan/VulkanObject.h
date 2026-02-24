//
// Created by deril on 2/23/26.
//

#pragma once

#include "VoxCore/Define.h"

VULKAN_NS
    template<typename T>
    struct is_vulkan_handle : std::false_type {};
    template<> struct is_vulkan_handle<VkBuffer> : std::true_type {};
    template<> struct is_vulkan_handle<VkImage>  : std::true_type {};
    template<> struct is_vulkan_handle<VkDevice> : std::true_type {};
    template<> struct is_vulkan_handle<VkInstance> : std::true_type{};
    template<> struct is_vulkan_handle<VkPhysicalDevice> : std::true_type{};
    template<> struct is_vulkan_handle<VkQueue> : std::true_type{};
    template<> struct is_vulkan_handle<VkSemaphore> : std::true_type{};
    template<> struct is_vulkan_handle<VkCommandBuffer> : std::true_type{};
    template<> struct is_vulkan_handle<VkFence> : std::true_type{};
    template<> struct is_vulkan_handle<VkDeviceMemory> : std::true_type{};
    template<> struct is_vulkan_handle<VkQueryPool> : std::true_type{};
    template<> struct is_vulkan_handle<VkImageView> : std::true_type{};
    template<> struct is_vulkan_handle<VkCommandPool> : std::true_type{};
    template<> struct is_vulkan_handle<VkRenderPass> : std::true_type{};
    template<> struct is_vulkan_handle<VkFramebuffer> : std::true_type{};
    template<> struct is_vulkan_handle<VkEvent> : std::true_type{};
    template<> struct is_vulkan_handle<VkBufferView> : std::true_type{};
    template<> struct is_vulkan_handle<VkShaderModule> : std::true_type{};
    template<> struct is_vulkan_handle<VkPipelineCache> : std::true_type{};
    template<> struct is_vulkan_handle<VkPipelineLayout> : std::true_type{};
    template<> struct is_vulkan_handle<VkPipeline> : std::true_type{};
    template<> struct is_vulkan_handle<VkDescriptorSetLayout> : std::true_type{};
    template<> struct is_vulkan_handle<VkSampler> : std::true_type{};
    template<> struct is_vulkan_handle<VkDescriptorSet> : std::true_type{};
    template<> struct is_vulkan_handle<VkDescriptorPool> : std::true_type{};
    template<> struct is_vulkan_handle<VkSwapchainKHR> : std::true_type{};
    template<> struct is_vulkan_handle<VkSurfaceKHR> : std::true_type{};

    template<typename HandleType>
    concept VulkanHandle = is_vulkan_handle<HandleType>::value;

    template<VulkanHandle T>
    class VulkanObject {
    protected:
        T mHandle = VK_NULL_HANDLE;
        VulkanObject(T handle) : mHandle(handle) {
        }

    public:
        inline T getHandle() const { return mHandle; }
        inline operator T() const { return mHandle; }
    };
NS_END
