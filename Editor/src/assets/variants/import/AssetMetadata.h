//
// Created by deril on 2/20/26.
//

#pragma once

#include <VoxCore/Pointers.h>
#include <VoxCore/math/UUID.h>
#include <VoxCore/math/Hash128.h>

#include "ImportSettings.h"
#include "../AssetType.h"

ASSETS_NS
    struct  AssetMetadata : Serializable {
        UUID uuid;
        uint32_t version;
        AssetType type;
        UPtr<ImportSettings> settings;

   AssetMetadata(const UUID uuid, const uint32_t version, const AssetType type, const Ref<ImportSettings> settings)
            : uuid(uuid),
              version(version),
              type(type),
              settings(settings) {
        }

        void serialize(FILE *file) override;
        static Expected<Ref<AssetMetadata>> deserialize(FILE *file);
    };
NS_END
