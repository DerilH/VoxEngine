//
// Created by deril on 2/25/26.
//

#pragma once


RENDER_NS
    struct TextureHandle {
        const uint32_t id;
        const uint32_t generation;
        explicit TextureHandle(uint32_t id, uint32_t generation);

        bool operator==(const TextureHandle &other) const;
    };
NS_END
namespace std {
    template<>
    struct hash<Vox::Render::TextureHandle> {
        std::size_t operator()(const Vox::Render::TextureHandle &p) const noexcept {
            return p.id ^ (p.generation << 1);
        }
    };
}