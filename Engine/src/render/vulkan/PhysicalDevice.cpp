//
// Created by deril on 2/12/26.
//


#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <vector>
#include <VoxEngine/render/vulkan/PhysicalDevice.h>
#include <VoxEngine/render/vulkan/Surface.h>
#include <vulkan/vulkan_core.h>


namespace Vox::Render::Vulkan {
    std::vector<VkExtensionProperties> getDeviceExtensions(const VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
        return availableExtensions;
    }

    SwapChainSupportDetails::SwapChainSupportDetails(const VkSurfaceCapabilitiesKHR &capabilities,
                                                     const std::vector<VkSurfaceFormatKHR> &formats,
                                                     const std::vector<VkPresentModeKHR> &presentModes) : mCapabilities(
                                                                                                              capabilities), mFormats(formats), mPresentModes(presentModes) {
    }

    PhysicalDevice::PhysicalDevice(const VkPhysicalDevice handle) : mHandle(handle) {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceMemoryProperties memoryProperties;
        vkGetPhysicalDeviceProperties(mHandle, &deviceProperties);
        vkGetPhysicalDeviceMemoryProperties(mHandle, &memoryProperties);
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            mRate += 1000;
        }

        VkDeviceSize totalMemory = 0;
        for (uint32_t i = 0; i < memoryProperties.memoryHeapCount; i++) {
            if (memoryProperties.memoryHeaps[i].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                totalMemory += memoryProperties.memoryHeaps[i].size;
            }
        }
        mTotalMemory = totalMemory;
        mRate += static_cast<int>(totalMemory / (1024 * 1024 * 256));

        mQueueFamilies = QueueFamilyRepository::findFamilies(handle, {GRAPHICS_QUEUE, TRANSFER_QUEUE});
        if (!mQueueFamilies.isComplete()) {
            mRate = -1;
        }
        mAvailableExtensions = getDeviceExtensions(mHandle);
    }

    bool PhysicalDevice::checkExtensions(const std::vector<std::string> &extensions) const {
        for (const auto &extension: extensions) {
            bool has = false;
            for (const auto &[extensionName, specVersion]: mAvailableExtensions) {
                if (strcmp(extension.c_str(), extensionName) == 0) {
                    has = true;
                    break;
                }
            }
            if (!has) return false;
        }
        return true;
    }

    //
    // bool PhysicalDevice::isSwapChainAdequate() const {
    //     return !mSwapChainDetails.formats().empty() && !mSwapChainDetails.presentModes().empty();
    // }

    VkPhysicalDevice PhysicalDevice::getHandle() const {
        return mHandle;
    }

    std::vector<VkExtensionProperties> PhysicalDevice::getAvailableExtensions() const {
        return mAvailableExtensions;
    }

    QueueFamilyRepository PhysicalDevice::getQueueFamilies() const {
        return mQueueFamilies;
    }

    int PhysicalDevice::getRate() const {
        return mRate;
    }

    VkDeviceSize PhysicalDevice::getTotalMemory() const {
        return mTotalMemory;
    }

    uint32_t PhysicalDevice::findMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties) const {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(mHandle, &memProperties);
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }

        throw std::runtime_error("failed to find suitable memory type!");
    }

    std::vector<PhysicalDevice> PhysicalDevice::pickDevices(const VkInstance instance, const std::vector<std::string> &neededExtensions) {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        VOX_CHECK(deviceCount != 0, "Failed to find GPUs with Vulkan support!");


        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        std::vector<PhysicalDevice> physicalDevices;
        for (const auto &device: devices) {
            PhysicalDevice d(device);
            if (!d.checkExtensions(neededExtensions)) continue;
            physicalDevices.push_back(std::move(d));
        }

        VOX_CHECK(!physicalDevices.empty(), "Failed to find a suitable GPU!");
        std::ranges::sort(physicalDevices, [](const PhysicalDevice &a, const PhysicalDevice &b) {
            return a.mRate < b.mRate;
        });

        return physicalDevices;
    }
}
