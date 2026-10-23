//
// Created by deril on 2/20/26.
//

#include "AssetMetadata.h"

ASSETS_NS
    void AssetMetadata::serialize(FILE *file) const {
        SERIALIZE(file, FIELD(uuid), FIELD(fileName), FIELD(version), FIELD(type));
    }

    Expected<Ref<AssetMetadata>> AssetMetadata::deserialize(FILE *file) {
        VOX_EXPECT(file != nullptr, "Invalid file for metadata deserialization");
        auto metadata = new AssetMetadata();
        auto ctx = contextForReading(file);
        deserializeInternal(file, ctx, "uuid", metadata->uuid, "fileName", metadata->fileName, "version", metadata->version, "type", metadata->type);
        return metadata;
    }

NS_END
