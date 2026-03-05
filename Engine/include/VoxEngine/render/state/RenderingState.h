//
// Created by deril on 3/4/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/containers/ArrayView.h"
#include "VoxEngine/render/Enums.h"

RENDER_NS
    class RenderingState {
        friend struct std::hash<Vox::Render::RenderingState>;
        ArrayView<Format> mColorFormats;
        Format mDepthFormat;
        Format mStencilFormat;


    public:
        RenderingState() = delete;

        RenderingState(ArrayView<Format> colorFormats, Format depthFormat, Format stencilFormat) : mColorFormats(colorFormats), mDepthFormat(depthFormat), mStencilFormat(stencilFormat) {
        }

        inline ArrayView<Format> getColorFormats() const { return mColorFormats; }

        inline Format getDepthFormat() const { return mDepthFormat; }

        inline Format getStencilFormat() const { return mStencilFormat; }

    };
NS_END

namespace std {
    template<>
    struct hash<Vox::Render::RenderingState> {
        inline size_t operator()(const Vox::Render::RenderingState& s) const noexcept {
            XXH64_state_t* state = XXH64_createState();
            XXH64_reset(state, 0);

            for (int i = 0; i < s.mColorFormats.size(); ++i) {
                Vox::Render::Format format = s.mColorFormats[i];
                XXH64_update(state, &format, sizeof(Vox::Render::Format));
            }

            XXH64_update(state, &s.mDepthFormat, sizeof(Vox::Render::Format));
            XXH64_update(state, &s.mStencilFormat, sizeof(Vox::Render::Format));

            uint64_t h = XXH64_digest(state);
            XXH64_freeState(state);
            return h;
        }
    };
}
