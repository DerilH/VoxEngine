//
// Created by deril on 2/19/26.
//

#pragma once

#include "Asset.h"
#include "MeshAsset.h"

ASSETS_NS
    class ModelAsset : public Asset {
    public:
        ModelAsset(InternedString path, Asset **nested, uint32_t nestedCount);

        Expected<Ref<MeshAsset>> getMesh(int index) const {
            return getNestedUnsafe<MeshAsset>(index);
        }

        AssetType type() const override {
            return StaticType();
        }

        static AssetType StaticType() {
            return AssetType::MODEL;
        }
    };

NS_END

namespace Vox::Resources {
    using ModelAsset = Vox::Editor::Assets::ModelAsset;
}
