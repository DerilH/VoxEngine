#pragma once
#include <fstream>
#include <VoxCore/containers/Containers.h>

namespace Vox::Render::Shaders {
    class ShaderSrc {
    public:
        const ShaderStage type;
        InternedString src;
        InternedString name;

        ShaderSrc(const ShaderStage type, InternedString src, InternedString name) : type(type), src(std::move(src)), name(std::move(name))
        {}

        ShaderSrc() = delete;

        static ShaderSrc load(const std::filesystem::path& path) {
            const std::ifstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file " + path.filename().string());
            }

            std::stringstream ss;
            ss << file.rdbuf();
            return {shaderTypeFrom(path), ss.str(), path.filename().string()};
        }
    };
}