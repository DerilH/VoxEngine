//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/Enums.h"
#include "VoxCore/containers/Containers.h"
#include "VoxEngine/render/Types.h"
#include "xxhash.h"

RENDER_NS
class ShaderState {
    friend struct std::hash<Vox::Render::ShaderState>;
    Vector<ShaderRef> mShaders;
    explicit ShaderState() = default;

public:
    class Builder {
        Vector<ShaderRef> mShaders{};
    public:
        Builder();

        Builder &shader(ShaderRef shader) &;

        Builder &shaders(const Vector<ShaderRef> &shaders) &;

        ShaderState build() const &;
    };

    static Builder GetBuilder();

    inline const Vector<ShaderRef> &shaders() const {return mShaders;}
};

NS_END

namespace std {
    template<>
    struct hash<Vox::Render::ShaderState> {
        inline size_t operator()(const Vox::Render::ShaderState& s) const noexcept {
            XXH64_state_t* state = XXH64_createState();
            XXH64_reset(state, 0);
            for (auto* shader: s.shaders()) {
                uint64_t ptr = (uint64_t)shader;
                XXH64_update(state, &ptr, sizeof(uint64_t));
            }
            uint64_t h = XXH64_digest(state);
            XXH64_freeState(state);
            return h;
        }
    };
}