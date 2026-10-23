//
// Created by deril on 2/19/26.
//

#include "RegularFileAsset.h"
#include <VoxCore/Assert.h>

ASSETS_NS
    RegularFileAsset::RegularFileAsset(ArrayView<void> data) : Asset(nullptr, 0), mData(data) {
        VOX_CHECK(!mData.isNull(), "Invalid data pointer");
    }

    RegularFileAsset::~RegularFileAsset() {
        if (!mData.isNull()) {
            delete[] static_cast<char*>(mData.pData);
        }
    }
NS_END
