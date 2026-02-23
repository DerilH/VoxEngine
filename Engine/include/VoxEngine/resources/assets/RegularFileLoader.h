//
// Created by deril on 2/19/26.
//

#pragma once

#include "AssetLoader.h"

RESOURCES_NS
class RegularFileLoader : public AssetLoader {
    Asset *load(std::string path, void *data, size_t dataSize) override;
};
NS_END
