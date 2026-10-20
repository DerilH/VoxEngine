//
// Created by deril on 2/19/26.
//

#pragma once

#include "../../../../Engine/include/VoxEngine/resources/Resource.h"
#include "MeshAsset.h"

RESOURCES_NS
    class MeshAsset;

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
