//
// Created by deril on 2/19/26.
//

#include "VoxEngine/resources/assets/RegularFile.h"

#include <VoxCore/containers/ArrayView.h>

#include "VoxEngine/resources/assets/Asset.h"

RESOURCES_NS
    RegularFile::RegularFile(std::string path, ArrayView<void> data) : Asset(std::move(path)), mData(data)
    {
        VOX_CHECK(!mData.isNull(), "Invalid data pointer");
    }

    RegularFile::~RegularFile() {
        delete[] static_cast<char*>(mData.pData);
    }
NS_END
