//
// Created by deril on 2/20/26.
//

#include "AssetMetadata.h"

ASSETS_NS
    void AssetMetadata::serialize(FILE *file) const {
        SERIALIZE(file, FIELD(uuid), FIELD(version), FIELD(type));
    }

    Expected<UPtr<AssetMetadata> > AssetMetadata::deserialize(FILE *file) {
        VOX_EXPECT(file != nullptr, "Invalid file for metadata deserialization");
        auto metadata = makeUPtr<AssetMetadata>();
        auto ctx = contextForReading(file);
        deserializeInternal(file, ctx, "uuid", metadata->uuid, "version", metadata->version, "type", metadata->type);
        return metadata;
    }

NS_END
