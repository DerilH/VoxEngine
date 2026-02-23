//
// Created by deril on 2/19/26.
//

#include "VoxEngine/resources/assets/RegularFile.h"
#include "VoxEngine/resources/assets/Asset.h"

RESOURCES_NS
    RegularFile::~RegularFile() {
        delete[] static_cast<char*>(mData);
    }

    void *RegularFile::getData() const {
        return mData;
    }

    RegularFile::RegularFile(std::string path, void* data, size_t dataSize) : Asset(std::move(path)), mData(data), mDataSize(dataSize)
    {
        VOX_CHECK(mData != nullptr, "Invalid data pointer");
    }

    size_t RegularFile::getDataSize() const {
        return mDataSize;
    }

    AssetType RegularFile::type() {
        return AssetType::REGULAR;
    }
NS_END
