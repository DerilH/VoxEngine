//
// Created by deril on 2/20/26.
//

#pragma once

#include "AssetLoader.h"
#include "VoxEngine/render/shaders/ShaderCompiler.h"

RESOURCES_NS
class ShaderLoader : public AssetLoader {
    static Vox::Render::Shaders::ShaderCompiler compiler;
    Asset *load(std::string path, ArrayView<void> data) override;
};
NS_END
