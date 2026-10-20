//
// Created by deril on 2/20/26.
//

#include "AssetMetadata.h"

ASSETS_NS
    void AssetMetadata::serialize(FILE *file) const {
        serializeInternal(file, uuid, version, type);
    }

    Expected<UPtr<AssetMetadata>> AssetMetadata::deserialize(FILE *file) {
        VOX_EXPECT(file != nullptr, "Invalid file for metadata deserialization");
        auto metadata = makeUPtr<AssetMetadata>();
        deserializeInternal(file, metadata->uuid, metadata->version, metadata->type);
        return metadata;
    }
NS_END
