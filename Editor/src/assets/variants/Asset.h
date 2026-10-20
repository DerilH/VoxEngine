//
// Created by deril on 2/19/26.
//

#pragma once

#include "../Serializable.h"
#include "AssetType.h"
#include <string>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>

ASSETS_NS
    class Asset {
    protected:
        InternedString mPath;
        InternedString mName;

        Asset **mNestedResources = nullptr;
        uint32_t mNestedResourceCount;

        NO_MOVE_DEFAULT(Asset);

        explicit Asset(const Asset &asset) = default;

        Asset &operator=(const Asset &asset) = default;

    public:
        explicit Asset(InternedString path, Asset **nested = nullptr, uint32_t nestedCount = 0);

        InternedString getPath() const {
            return mPath;
        }

        InternedString getName() const {
            return mName;
        }

        virtual ~Asset() = 0;

        template<typename AssetType, typename = std::enable_if_t<std::is_base_of_v<Asset, AssetType> > >
        AssetType *getNestedUnsafe(int id) const {
            return reinterpret_cast<AssetType *>(mNestedResources[id]);
        }

        template<typename AssetType, typename = std::enable_if_t<std::is_base_of_v<Asset, AssetType> > >
        Expected<Ref<AssetType>> getNested(int id) const {
            VOX_EXPECT(id < mNestedResourceCount, "Nested id({}) is out of bounds", id);
            auto asset = mNestedResources[id];
            VOX_EXPECT(asset->type() == AssetType::StaticType(), "Nested asset type mismatch, expected {}, actual {}", AssetType::StaticType(), asset->type());
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
