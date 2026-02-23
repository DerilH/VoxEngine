#pragma once
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "QueueFamilyRepository.h"
#include "Surface.h"
#include "VulkanObject.h"


VULKAN_NS
    class PhysicalDevice : public VulkanObject<VkPhysicalDevice>{
        std::vector<VkExtensionProperties> mAvailableExtensions;
        QueueFamilyRepository mQueueFamilies;
        int mRate = 0;
        unsigned long mTotalMemory = 0;

        explicit PhysicalDevice(VkPhysicalDevice handle);
    public:

        bool checkExtensions(const std::vector<std::string> &extensions) const;
        static std::vector<PhysicalDevice> pickDevices(VkInstance instance, const std::vector<std::string> &neededExtensions = {});

        std::vector<VkExtensionProperties> getAvailableExtensions() const;
        QueueFamilyRepository getQueueFamilies() const;
        int getRate() const;
        VkDeviceSize getTotalMemory() const;
        uint32_t findMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties) const;
    };
NS_END