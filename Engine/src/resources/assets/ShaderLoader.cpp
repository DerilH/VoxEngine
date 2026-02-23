//
// Created by deril on 2/20/26.
//

#include "VoxEngine/resources/assets/ShaderLoader.h"
#include "VoxEngine/resources/assets/ShaderAsset.h"
#include <string>
RESOURCES_NS
    Vox::Render::Shader::ShaderCompiler ShaderLoader::compiler;

using namespace Render::Shader;
Asset *ShaderLoader::load(std::string path, void *data, size_t dataSize) {
    ShaderSrc src = ShaderSrc(Render::Shader::shaderTypeFrom(path), std::string((const char*)data, dataSize), path);
    CompiledShader shader = compiler.compile(src);
    size_t s = shader.bin.size() * sizeof(uint32_t);
    uint32_t* cpy = static_cast<uint32_t *>(malloc(s));
    memcpy(cpy,shader.bin.data(), s);
    return new ShaderAsset(path, new Buffer(cpy, shader.bin.size()));
}
NS_END
