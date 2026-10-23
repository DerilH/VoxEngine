//
// Created by deril on 2/20/26.
//

#pragma once

#include <VoxCore/Pointers.h>
#include <VoxCore/math/UUID.h>
#include <VoxCore/math/Hash128.h>
#include <VoxCore/serialization/Serializable.h>

#include "ImportSettings.h"
#include "../AssetType.h"

ASSETS_NS
    struct AssetMetadata : public Resources::Serializable<Resources::JsonSerialization> {
        UUID uuid{};
        InternedString fileName;
        uint32_t version{1};
        AssetType type{AssetType::REGULAR};
        UPtr<ImportSettings> settings{nullptr};

        AssetMetadata() = default;

        AssetMetadata(const UUID uuid, const uint32_t version, const AssetType type, UPtr<ImportSettings> settings = nullptr)
            : uuid(uuid),
              version(version),
              type(type),
              settings(std::move(settings)) {
        }

        AssetMetadata(const AssetMetadata &other)
            : uuid(other.uuid),
              fileName(other.fileName),
              version(other.version),
              type(other.type),
              settings(other.settings ? other.settings->clone() : nullptr) {
        }

        void serialize(FILE *file) const override;

        static Expected<UPtr<AssetMetadata> > deserialize(FILE *file);
    };

NS_END
