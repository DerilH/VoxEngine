#pragma once
#include <fstream>
#include <VoxCore/containers/Containers.h>

namespace Vox::Render::Shaders {
    class ShaderSrc {
    public:
        InternedString src;

        ShaderSrc(InternedString src) : src(std::move(src))
        {}

        ShaderSrc() = delete;

        static ShaderSrc load(const std::filesystem::path& path) {
            const std::ifstream file(path);
            if (!file.is_open()) {
                throw std::runtime_error("Failed to open file " + path.filename().string());
            }

            std::stringstream ss;
            ss << file.rdbuf();
            return {ss.str()};
        }
    };
}