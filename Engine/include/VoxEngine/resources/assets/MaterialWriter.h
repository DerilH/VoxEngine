//
// Created by deril on 10/5/26.
//

#pragma once
#include <VoxCore/Define.h>

#include "AssetWriter.h"


RESOURCES_NS
    class MaterialWriter : public AssetWriter {
    public:
        void writeToFile(const std::filesystem::path &path, Asset* asset) override;
    };

NS_END
