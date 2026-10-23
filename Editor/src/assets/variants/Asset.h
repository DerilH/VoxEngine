//
// Created by deril on 2/19/26.
//

#pragma once

#include "AssetType.h"
#include "../../Define.h"
#include <VoxCore/Pointers.h>

ASSETS_NS
    class Asset {
    protected:
        Asset **mNestedResources = nullptr;
        uint32_t mNestedResourceCount = 0;

        NO_MOVE_DEFAULT(Asset);

        explicit Asset(const Asset &asset) = default;

        Asset &operator=(const Asset &asset) = default;

    public:
        explicit Asset(Asset **nested, uint32_t nestedCount);

        virtual ~Asset();

        template<typename AssetType, typename = std::enable_if_t<std::is_base_of_v<Asset, AssetType>>>
        AssetType *getNestedUnsafe(int id) const {
            return reinterpret_cast<AssetType *>(mNestedResources[id]);
        }

        template<typename AssetType, typename = std::enable_if_t<std::is_base_of_v<Asset, AssetType>>>
        Expected<Ref<AssetType>> getNested(int id) const {
            VOX_EXPECT(id < mNestedResourceCount, "Nested id({}) is out of bounds", id);
            auto asset = mNestedResources[id];
            VOX_EXPECT(asset->type() == AssetType::StaticType(), "Nested asset type mismatch, expected {}, actual {}", +AssetType::StaticType(), +asset->type());
            return reinterpret_cast<AssetType *>(mNestedResources[id]);
        }

        Asset **getNested() const {
            return mNestedResources;
        }

        bool hasNested() const;

        uint32_t getNestedCount() const;

        virtual AssetType type() const = 0;

        virtual UPtr<Asset> clone() const;

        virtual void cloneTo(void *ptr) const;
    };

NS_END
