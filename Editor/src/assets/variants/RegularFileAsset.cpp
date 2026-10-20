//
// Created by deril on 2/19/26.
//

#include "../RegularFile.h"

#include <VoxCore/containers/ArrayView.h>

#include "Asset.h"

RESOURCES_NS
    RegularFile::RegularFile(InternedString path, ArrayView<void> data) : Asset(path), mData(data)
    {
        VOX_CHECK(!mData.isNull(), "Invalid data pointer");
    }

    RegularFile::~RegularFile() {
        delete[] static_cast<char*>(mData.pData);
    }
NS_END
