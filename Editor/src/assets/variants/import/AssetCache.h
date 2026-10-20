#pragma once
#include <VoxCore/math/Hash128.h>

#include "../../Serializable.h"

ASSETS_NS
    struct AssetCache : public Serializable {
        Hash128 hash;
        uint64_t mtime;
        uint64_t size;

        AssetCache(Hash128 hash, uint64_t mtime, uint64_t size)
            : hash(hash),
              mtime(mtime),
              size(size) {
        }

        void serialize(FILE *file) const override;

        static AssetCache deserialize(FILE *file);
    };

NS_END
