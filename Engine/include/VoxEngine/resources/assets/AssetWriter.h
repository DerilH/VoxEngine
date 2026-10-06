//
// Created by deril on 2/19/26.
//

#pragma once
#include "VoxCore/Define.h"
#include "Asset.h"
#include <filesystem>
#include <VoxCore/Pointers.h>

RESOURCES_NS
    class AssetWriter {
    public:
        virtual ~AssetWriter() = default;
        virtual void writeToFile(const std::filesystem::path &path, Asset* asset) = 0;
    };

NS_END
