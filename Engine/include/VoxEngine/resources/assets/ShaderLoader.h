//
// Created by deril on 2/20/26.
//

#pragma once

#include "AssetLoader.h"
#include "VoxEngine/render/shaders/ShaderCompiler.h"

RESOURCES_NS
class ShaderLoader : public AssetLoader {
    static Vox::Render::Shader::ShaderCompiler compiler;
    Asset *load(std::string path, void *data, size_t dataSize) override;

};
NS_END
