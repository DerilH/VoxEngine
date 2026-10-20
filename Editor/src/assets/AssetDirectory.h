#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/math/UUID.h>

#include "../Define.h"

ASSETS_NS
    class AssetDirectory {
        LinkedList<UPtr<AssetDirectory>> mNested;
        LinkedList<UUID> mAssets;

    public:
        const InternedString path;
        const InternedString name;

        AssetDirectory(InternedString path, InternedString name) : path(path), name(name) {
        }

        void addAsset(UUID asset) {
            mAssets.emplace_back(asset);
        }

        void removeAsset(UUID asset) {
            mAssets.remove(asset);
        }

        void addDirectory(UPtr<AssetDirectory> dir) {
            mNested.emplace_back(std::move(dir));
        }

        void removeDir(InternedString path) {
            mNested.remove_if([path](const UPtr<AssetDirectory> &a) {
                return a->path == path;
            });
        }

        const LinkedList<UPtr<AssetDirectory> > &getNested() const {
            return mNested;
        }

        const LinkedList<UUID> &getAssets() const {
            return mAssets;
        }
    };

NS_END
