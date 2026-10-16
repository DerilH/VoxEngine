//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/containers/Containers.h"
#include "VoxEngine/render/Types.h"
#include <xxh3.h>
#include <VoxCore/render/Enums.h>

RENDER_NS
    class ShaderState {
        friend struct std::hash<ShaderState>;
        HashMap<ShaderStage, ShaderRef> mShaders;

        explicit ShaderState() = default;

    public:
        class Builder {
            HashMap<ShaderStage, ShaderRef> mShaders{};
        public:
            Builder();

            Builder& shader(ShaderRef shader);

            Builder& shaders(const Vector<ShaderRef>& shaders);

            Expected<ShaderState> build() const&;
        };

        static Builder GetBuilder();

        inline const HashMap<ShaderStage, ShaderRef>& shaders() const { return mShaders; }

        inline bool operator==(const ShaderState& other) const {
            bool same = mShaders.size() == other.mShaders.size();
            for (int i = 0; i < mShaders.size(); ++i) {
//                same = same && mShaders[i] == other.mShaders[i];
            }
            return same;
        }
    };

NS_END

namespace std {
    template<>
    struct hash<Vox::Render::ShaderState> {
        inline size_t operator()(const Vox::Render::ShaderState& s) const noexcept {
            XXH64_state_t* state = XXH64_createState();
            XXH64_reset(state, 0);
//            for (auto* shader: s.shaders()) {
//                uint64_t ptr = (uint64_t)shader;
//                XXH64_update(state, &ptr, sizeof(uint64_t));
//            }
//            XXH64_update(state, , sizeof(uint64_t));
            uint64_t h = XXH64_digest(state);
            XXH64_freeState(state);
            return h;
        }
    };
}