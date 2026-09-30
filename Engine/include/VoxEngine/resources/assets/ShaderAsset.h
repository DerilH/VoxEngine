//
// Created by deril on 2/20/26.
//

#pragma once


#include "Asset.h"
#include "VoxCore/containers/ArrayView.h"
#include "VoxEngine/render/shaders/CompiledShader.h"

RESOURCES_NS
class ShaderAsset : public Asset {
    const Render::Shaders::CompiledShader mCompiled;
public:
    explicit ShaderAsset(std::string path, const Render::Shaders::CompiledShader mCompiled);
    const Render::Shaders::CompiledShader& getCompiled() const;

    AssetType type() override;
};
NS_END
