#pragma once
#include <filesystem>
#include <stdexcept>
#include <string>
#include <shaderc/shaderc.h>
#include <VoxEngine/render/Enums.h>

namespace Vox::Render::Shaders {
    inline shaderc_shader_kind vox2shaderc(ShaderStage type) {
        switch (type) {
            case ShaderStage::VERTEX_SHADER:
                return shaderc_vertex_shader;
            case ShaderStage::FRAGMENT_SAHDER:
                return shaderc_fragment_shader;
            default: throw std::invalid_argument("Unknown shader type");
        }
    }
    inline ShaderStage shaderTypeFrom(std::string path) {
        if (path.ends_with(".vert")) return ShaderStage::VERTEX_SHADER;
        if (path.ends_with(".frag")) return ShaderStage::FRAGMENT_SAHDER;
        throw std::invalid_argument("Unknown shader type");
    }

    inline ShaderStage shaderTypeFrom(const std::filesystem::path &path) {
        return shaderTypeFrom(path.extension().string());
    }

    inline bool isValidType(const std::filesystem::path &path) {
        try {
            shaderTypeFrom(path);
            return true;
        } catch (const std::invalid_argument &e) {
            return false;
        }
    }


}
