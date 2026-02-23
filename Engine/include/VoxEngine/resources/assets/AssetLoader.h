//
// Created by deril on 2/19/26.
//

#pragma once
#include "VoxCore/Define.h"
#include "Asset.h"
#include <filesystem>

RESOURCES_NS
class AssetLoader {
public:
    virtual Asset *load(std::string path, void *data, size_t dataSize) = 0;
    Asset* fromFile(const std::filesystem::path& path);
};
NS_END
