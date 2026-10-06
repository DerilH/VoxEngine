//
// Created by deril on 10/6/26.
//

#pragma once
#include <VoxEngine/resources/assets/Asset.h>

namespace Vox::Editor {
    class AssetIcon {
    public:
        ConstRef<Resources::Asset> mAsset;

    public:
        explicit AssetIcon(ConstRef<Resources::Asset> asset);

        void render() const;
    };
}
