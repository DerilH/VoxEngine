//
// Created by deril on 2/20/26.
//

#include "AssetMetadata.h"

ASSETS_NS
    void AssetMetadata::serialize(FILE *file) const {
        SerializationContext<Json> ctx;
        serializeInternal(file, ctx, "uuid", uuid, "version", version, "type", type);
        ctx.flush(file);
    }

    Expected<UPtr<AssetMetadata>> AssetMetadata::deserialize(FILE *file) {
        VOX_EXPECT(file != nullptr, "Invalid file for metadata deserialization");
        auto metadata = makeUPtr<AssetMetadata>();
        auto ctx = SerializationContext<Json>::createForReading(file);
        if (ctx.readRoot) {
            deserializeInternal(file, ctx, "uuid", metadata->uuid, "version", metadata->version, "type", metadata->type);
            return metadata;
        }
        VOX_EXPECT(false, "Invalid metadata file, root not found")
    }
NS_END
