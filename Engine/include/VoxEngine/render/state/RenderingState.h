//
// Created by deril on 3/4/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/containers/ArrayView.h"
#include "VoxCore/render/Enums.h"
#include <xxh3.h>

RENDER_NS
    class RenderingState {
        friend struct std::hash<RenderingState>;
        Array<Format, 8> mColorFormats;
        uint8_t mFormatsCount;
        Format mDepthFormat;
        Format mStencilFormat;
        mutable uint64_t mCachedHash = 0;

    public:
        RenderingState() = delete;

        RenderingState(ArrayView<Format> formats, Format depthFormat, Format stencilFormat) : mColorFormats(formats.copy<8>()), mFormatsCount(formats.size()), mDepthFormat(depthFormat), mStencilFormat(stencilFormat) {
        }

        ArrayView<const Format> getColorFormats() const { return ArrayView(mColorFormats.data(), mFormatsCount); }

        Format getDepthFormat() const { return mDepthFormat; }

        Format getStencilFormat() const { return mStencilFormat; }

        bool operator==(const RenderingState &other) const {
            bool same = mDepthFormat == other.mDepthFormat && mStencilFormat == other.mStencilFormat;
            for (int i = 0; i < mColorFormats.size(); ++i) {
                same = same && mColorFormats[i] == other.mColorFormats[i];
            }
            return same;
        }
    };

NS_END

namespace std {
    template<>
    struct hash<Vox::Render::RenderingState> {
        inline size_t operator()(const Vox::Render::RenderingState &s) const noexcept {
            if (s.mCachedHash != 0) return s.mCachedHash;
            XXH64_state_t *state = XXH64_createState();
            XXH64_reset(state, 0);

            for (int i = 0; i < s.mColorFormats.size(); ++i) {
                Vox::Render::Format format = s.mColorFormats[i];
                XXH64_update(state, &format, sizeof(Vox::Render::Format));
            }

            XXH64_update(state, &s.mDepthFormat, sizeof(Vox::Render::Format));
            XXH64_update(state, &s.mStencilFormat, sizeof(Vox::Render::Format));

            s.mCachedHash = XXH64_digest(state);
            XXH64_freeState(state);
            return s.mCachedHash;
        }
    };
}
