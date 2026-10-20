#pragma once
#include <VoxCore/math/Hash128.h>
#include "../../Serializable.h"

ASSETS_NS
    struct AssetCache : public Serializable<Json> {
        Hash128 hash{0, 0};
        uint64_t mtime{0};
        uint64_t size{0};

        AssetCache() = default;

        AssetCache(Hash128 hash, uint64_t mtime, uint64_t size)
            : hash(hash),
              mtime(mtime),
              size(size) {
        }

        void serialize(FILE *file) const override {
            SerializationContext<Json> ctx;
            serializeInternal(file, ctx, "hash", hash, "mtime", mtime, "size", size);
            ctx.flush(file);
        }

        static Expected<AssetCache> deserialize(FILE *file) {
            VOX_EXPECT(file != nullptr, "Invalid file for metadata deserialization");
            auto metadata = AssetCache();
            auto ctx = SerializationContext<Json>::createForReading(file);
            if (ctx.readRoot) {
                deserializeInternal(file, ctx, "hash", metadata.hash, "mtime", metadata.mtime, "size", metadata.size);
                return metadata;
            }
            VOX_EXPECT(false, "Invalid metadata file, root not found")
        }
    };
NS_END
