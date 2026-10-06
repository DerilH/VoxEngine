#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/Pointers.h>
RESOURCES_NS
    class AssetDirectory {
        LinkedList<UPtr<AssetDirectory> > mNested;

    public:
        const InternedString path;
        const InternedString name;

        AssetDirectory(InternedString path, InternedString name) : path(path), name(name) {
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
    };

NS_END
