//
// Created by deril on 2/19/26.
//

#pragma once

#include "VoxEngine/resources/serialization/Serializable.h"
#include "AssetType.h"
#include <string>

RESOURCES_NS
    class Asset : public Serialization::Serializable {
        std::string mPath;

        Asset **mNestedAssets = nullptr;
        uint32_t mNestedAssetsCount;

        NON_COPYABLE_NON_MOVABLE(Asset);
    public:
        explicit Asset(std::string path, Asset **nested = nullptr, uint32_t nestedCount = 0);

        std::string getPath() const;

        virtual ~Asset() = 0;


        template<typename AssetType, typename = std::enable_if_t<std::is_base_of_v<Asset, AssetType>>>
        AssetType *getNested(int id) const {
            VOX_CHECK(id < getNestedCount(), "Nested asset index out of bound");
            return reinterpret_cast<AssetType *>(mNestedAssets[id]);
        }

        bool hasNested() const;

        uint32_t getNestedCount() const;

        virtual AssetType type() = 0;

        void serialize(FILE *file) override;

    };
NS_END
