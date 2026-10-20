//
// Created by deril on 2/19/26.
//

#pragma once
#include <filesystem>
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include "../Asset.h"

ASSETS_NS
    class AssetWriter {
    public:
        virtual ~AssetWriter() = default;
        virtual void writeToFile(const std::filesystem::path &path, Asset* asset) = 0;
    };
NS_END
