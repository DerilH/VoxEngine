//
// Created by deril on 2/20/26.
//

#pragma once
#include <cstdint>
#include <optional>
#include <VoxCore/Define.h>
#include <VoxCore/Assert.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>
#include "ResourceHandle.h"

RESOURCES_NS
    template<typename T>
    class ResourceSlotPool {
    public:
        struct Slot {
            std::optional<T> resource{std::nullopt};
            uint16_t generation{1};
            bool occupied{false};
        };

    private:
        Vector<Slot> mSlots;
        Vector<uint32_t> mFreeList;
        uint32_t mActiveCount{0};

    public:
        ResourceSlotPool() = default;
        ~ResourceSlotPool() = default;

        NO_COPY_MOVE(ResourceSlotPool);

        Expected<ResourceHandle<T>> allocate(T &&resource) {
            uint32_t index = 0;
            if (!mFreeList.empty()) {
                index = mFreeList.back();
                mFreeList.pop_back();
                auto &slot = mSlots[index];
                slot.resource.emplace(std::move(resource));
                slot.occupied = true;
                mActiveCount++;
                return ResourceHandle<T>(index, slot.generation);
            } else {
                index = static_cast<uint32_t>(mSlots.size());
                Slot &slot = mSlots.emplace_back();
                slot.generation = 1;
                slot.occupied = true;
                slot.resource.emplace(std::move(resource));
                mActiveCount++;
                return ResourceHandle<T>(index, 1);
            }
        }

        bool isValid(ResourceHandle<T> handle) const noexcept {
            if (!handle.isValid()) return false;
            if (handle.index >= mSlots.size()) return false;
            const auto &slot = mSlots[handle.index];
            return slot.occupied && slot.generation == handle.generation;
        }

        Expected<Ref<T>> get(ResourceHandle<T> handle) {
            VOX_EXPECT(handle.isValid(), "Invalid handle provided");
            VOX_EXPECT(handle.index < mSlots.size(), "Handle index out of bounds: {}", handle.index);
            auto &slot = mSlots[handle.index];
            VOX_EXPECT(slot.occupied, "Slot at index {} is unoccupied", handle.index);
            VOX_EXPECT(slot.generation == handle.generation, "Stale handle generation: slot {}, handle {}", slot.generation, handle.generation);
            return &slot.resource.value();
        }

        Expected<ConstRef<T>> get(ResourceHandle<T> handle) const {
            VOX_EXPECT(handle.isValid(), "Invalid handle provided");
            VOX_EXPECT(handle.index < mSlots.size(), "Handle index out of bounds: {}", handle.index);
            const auto &slot = mSlots[handle.index];
            VOX_EXPECT(slot.occupied, "Slot at index {} is unoccupied", handle.index);
            VOX_EXPECT(slot.generation == handle.generation, "Stale handle generation: slot {}, handle {}", slot.generation, handle.generation);
            return &slot.resource.value();
        }

        bool release(ResourceHandle<T> handle) {
            if (!isValid(handle)) return false;
            auto &slot = mSlots[handle.index];
            slot.resource.reset();
            slot.occupied = false;
            slot.generation++;
            if (slot.generation == 0) slot.generation = 1;
            mFreeList.push_back(handle.index);
            mActiveCount--;
            return true;
        }

        void clear() {
            for (uint32_t i = 0; i < mSlots.size(); ++i) {
                if (mSlots[i].occupied) {
                    mSlots[i].resource.reset();
                    mSlots[i].occupied = false;
                    mSlots[i].generation++;
                    if (mSlots[i].generation == 0) mSlots[i].generation = 1;
                    mFreeList.push_back(i);
                }
            }
            mActiveCount = 0;
        }

        uint32_t capacity() const noexcept { return static_cast<uint32_t>(mSlots.size()); }
        uint32_t size() const noexcept { return mActiveCount; }
        bool empty() const noexcept { return mActiveCount == 0; }
    };
NS_END
