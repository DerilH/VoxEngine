//
// Created by deril on 2/20/26.
//

#include "VoxEngine/resources/assets/ShaderLoader.h"
#include "VoxEngine/resources/assets/ShaderAsset.h"
#include <string>
RESOURCES_NS
    Vox::Render::Shaders::ShaderCompiler ShaderLoader::compiler;

Asset *ShaderLoader::load(std::string path, void *data, size_t dataSize) {
    Render::Shaders::ShaderSrc src = Render::Shaders::ShaderSrc(Render::Shaders::shaderTypeFrom(path), std::string((const char*)data, dataSize), path);
    Render::Shaders::CompiledShader shader = compiler.compile(src);
    size_t s = shader.bin.size() * sizeof(uint32_t);
    uint32_t* cpy = static_cast<uint32_t *>(malloc(s));
    memcpy(cpy,shader.bin.data(), s);
    return new ShaderAsset(path, new ArrayView(cpy, shader.bin.size()));
}
NS_END
