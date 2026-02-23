//
// Created by deril on 2/12/26.
//

#include "../../../include/VoxEngine/render/shaders/ShaderRepository.h"

#include <ranges>

namespace Vox::Render::Shader {
    ShaderRepository::ShaderRepository() {
    }

    ShaderRepository::~ShaderRepository() {
    }

    void ShaderRepository::add(const CompiledShader &shader) {
        mShaders.insert({shader.name, shader});
    }

    std::optional<std::reference_wrapper<CompiledShader>> ShaderRepository::get(const std::string &name) {
        const auto it = mShaders.find(name);
        return it == mShaders.end() ? std::nullopt : std::optional<std::reference_wrapper<CompiledShader>>(it->second);
    }
}
