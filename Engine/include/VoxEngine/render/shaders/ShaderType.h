#pragma once
#include <filesystem>
#include <stdexcept>
#include <string>
#include <shaderc/shaderc.h>

namespace Vox::Render::Shader {
    enum class ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY
    };

    inline shaderc_shader_kind vox2shaderc(ShaderType type) {
        switch (type) {
            case ShaderType::VERTEX:
                return shaderc_vertex_shader;
            case ShaderType::FRAGMENT:
                return shaderc_fragment_shader;
            case ShaderType::GEOMETRY:
                return shaderc_geometry_shader;
            default: throw std::invalid_argument("Unknown shader type");
        }
    }
    inline ShaderType shaderTypeFrom(std::string path) {
        if (path.ends_with(".vert")) return ShaderType::VERTEX;
        if (path.ends_with(".frag")) return ShaderType::FRAGMENT;
        if (path.ends_with(".geom")) return ShaderType::GEOMETRY;
        throw std::invalid_argument("Unknown shader type");
    }

    inline ShaderType shaderTypeFrom(const std::filesystem::path &path) {
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
