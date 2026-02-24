#pragma once

#include <format>
#include <map>
#include <optional>
#include <ranges>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <vulkan/vulkan_core.h>

#include "QueueType.h"

VULKAN_NS
    class QueueFamily {
        QueueType mType;
        int mIndex;
    public:

        explicit QueueFamily(int index, QueueType type);

        int index() const;

        QueueType type() const;

        bool operator<(const QueueFamily &other) const;
    };

    class QueueFamilyRepository {

        VkPhysicalDevice mDeviceHandle = nullptr;
        std::unordered_map<VkSurfaceKHR, QueueFamily> mPresentBySurface;
        std::unordered_map<QueueType, QueueFamily> mData;

        QueueFamilyRepository() = default;

        explicit QueueFamilyRepository(VkPhysicalDevice deviceHandle);

        friend class PhysicalDevice;

    public:
        static QueueFamilyRepository findFamilies(VkPhysicalDevice device, std::set<QueueType> neededQueues);

        bool isComplete() const;

        QueueFamily operator[](QueueType type) const;

        const std::unordered_map<QueueType, QueueFamily> &data() const;

        auto getUniqueFamilies() const -> decltype(mData | std::views::values);
    };
NS_END
