//
// Created by deril on 2/13/26.
//

#include <ranges>
#include <VoxEngine/render/vulkan/QueueFamilyRepository.h>
#include <vulkan/vulkan_core.h>

namespace Vox::Render::Vulkan {
    QueueFamily::QueueFamily(const int index, const QueueType type) : mType(type), mIndex(index) {
    }

    int QueueFamily::index() const {
        return mIndex;
    }

    QueueType QueueFamily::type() const {
        return mType;
    }

    bool QueueFamily::operator<(const QueueFamily &other) const {
        return mIndex < other.mIndex;
    }

    QueueFamilyRepository::QueueFamilyRepository(VkPhysicalDevice deviceHandle) : mDeviceHandle(deviceHandle){
    }

    QueueFamilyRepository QueueFamilyRepository::findFamilies(const VkPhysicalDevice device, std::set<QueueType> neededQueues) {
        QueueFamilyRepository indices(device);

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto &queueFamily: queueFamilies) {
            for (const auto &queueType: neededQueues) {
                if (queueFamily.queueFlags & queueType) {
                    neededQueues.erase(queueType);
                    indices.mData.emplace(queueType, QueueFamily(i, queueType));
                    break;
                }
            }

            if (indices.isComplete()) {
                break;
            }

            i++;
        }

        return indices;
    }

    bool QueueFamilyRepository::isComplete() const {
        return mData.contains(GRAPHICS_QUEUE) && mData.contains(TRANSFER_QUEUE);
    }

    QueueFamily QueueFamilyRepository::operator[](const QueueType type) const {
        try {
            return mData.at(type);
        } catch (const std::out_of_range &e) {
            LOG_ERROR("No queue family find for type: {}", static_cast<int>(type));
            throw e;
        }
    }

    const std::unordered_map<QueueType, QueueFamily>& QueueFamilyRepository::data() const {
        return mData;
    }

    auto QueueFamilyRepository::getUniqueFamilies() const -> decltype(mData | std::views::values) {
        return mData | std::views::values;
    }
}
