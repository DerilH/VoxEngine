//
// Created by deril on 2/20/26.
//

#pragma once


#include "Asset.h"
#include "VoxCore/containers/Buffer.h"

RESOURCES_NS
class ShaderAsset : public Asset {
    const Buffer<uint32_t> * mCompiled;
public:
    explicit ShaderAsset(std::string path, const Buffer<uint32_t> *mCompiled);
    const Buffer<uint32_t>& getCompiled() const;

    AssetType type() override;
};
NS_END
