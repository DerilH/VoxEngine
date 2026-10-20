#pragma once
#include <VoxCore/math/Hash128.h>
#include "../../Serializable.h"

ASSETS_NS
    struct AssetCache : public Serializable<Binary> {
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
            serializeInternal(file, hash, mtime, size);
        }

        static AssetCache deserialize(FILE *file) {
            AssetCache cache{};
            deserializeInternal(file, cache.hash, cache.mtime, cache.size);
            return cache;
        }
    };
NS_END
