//
// Created by deril on 2/19/26.
//

#pragma once
#include "Asset.h"
RESOURCES_NS
class RegularFile : public Asset{
    void* const mData;
    size_t mDataSize;
public:
    RegularFile(std::string path, void* data, size_t dataSize);

    size_t getDataSize() const;
    void* getData() const;
    ~RegularFile() override;

    AssetType type() override;
};
NS_END

