//
// Created by deril on 2/19/26.
//

#pragma once

#include "VoxEngine/resources/serialization/Serializable.h"
#include "AssetType.h"
#include <string>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>

RESOURCES_NS
    class Asset : public Serialization::Serializable {
    protected:
        InternedString mPath;
        InternedString mName;

        Asset **mNestedAssets = nullptr;
        uint32_t mNestedAssetsCount;

        NO_MOVE_DEFAULT(Asset);
        explicit Asset(const Asset& asset) = default;
        Asset& operator=(const Asset& asset) = default;
    public:

        explicit Asset(InternedString path, Asset **nested = nullptr, uint32_t nestedCount = 0);

        InternedString getPath() const {
            return mPath;
        }

        InternedString getName() const {
            return mName;
        }

        virtual ~Asset() = 0;

        template<typename AssetType, typename = std::enable_if_t<std::is_base_of_v<Asset, AssetType>>>
        AssetType *getNested(int id) const {
            VOX_CHECK(id < getNestedCount(), "Nested asset index out of bound");
            return reinterpret_cast<AssetType *>(mNestedAssets[id]);
        }

        Asset** getNested() const {
            return mNestedAssets;
        }

        bool hasNested() const;

        uint32_t getNestedCount() const;

        virtual AssetType type() const = 0;

        virtual UPtr<Asset> clone() const;
        virtual void cloneTo(void* ptr) const;
        void serialize(FILE *file) override;

    };
NS_END
