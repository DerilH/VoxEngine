#pragma once
#include <VoxCore/math/Hash128.h>
#include <VoxCore/serialization/Serializable.h>

#include "../../JsonSerialization.h"

ASSETS_NS
    struct AssetCache : public Resources::Serializable<Resources::JsonSerialization> {
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
            auto ctx = contextForWriting(file);
            serializeInternal(file, ctx, "hash", hash, "mtime", mtime, "size", size);
            ctx.flush(file);
        }

        static Expected<AssetCache> deserialize(FILE *file) {
            VOX_EXPECT(file != nullptr, "Invalid file for metadata deserialization");
            auto metadata = AssetCache();
            auto ctx = contextForReading(file);
            deserializeInternal(file, ctx, "hash", metadata.hash, "mtime", metadata.mtime, "size", metadata.size);
            return metadata;
        }
    };

NS_END
