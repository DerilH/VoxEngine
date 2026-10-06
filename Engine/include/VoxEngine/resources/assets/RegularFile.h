//
// Created by deril on 2/19/26.
//

#pragma once
#include <VoxCore/containers/ArrayView.h>

#include "Asset.h"
RESOURCES_NS
class RegularFile : public Asset{
    ArrayView<void> mData;
public:
    RegularFile(std::string path, ArrayView<void> data);

    ArrayView<void> getData() const {
        return mData;
    }
    ~RegularFile() override;

    AssetType type() const override {
        return StaticType();
    }

    static AssetType StaticType() {
        return AssetType::REGULAR;
    }
};
NS_END

