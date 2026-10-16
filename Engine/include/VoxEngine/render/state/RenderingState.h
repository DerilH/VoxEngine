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

        RenderingState(ArrayView<Format> formats, Format depthFormat, Format stencilFormat) : mFormatsCount(formats.size()), mDepthFormat(depthFormat), mStencilFormat(stencilFormat) {
            mColorFormats.fill(Format::UNDEFINED);
            for (size_t i = 0; i < formats.size() && i < 8; ++i) {
                mColorFormats[i] = formats[i];
            }
        }

        ArrayView<const Format> getColorFormats() const { return ArrayView(mColorFormats.data(), mFormatsCount); }

        Format getDepthFormat() const { return mDepthFormat; }

        Format getStencilFormat() const { return mStencilFormat; }

        bool operator==(const RenderingState &other) const {
            if (mDepthFormat != other.mDepthFormat || mStencilFormat != other.mStencilFormat || mFormatsCount != other.mFormatsCount)
                return false;
            for (int i = 0; i < mFormatsCount; ++i) {
                if (mColorFormats[i] != other.mColorFormats[i]) return false;
            }
            return true;
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

            for (int i = 0; i < s.mFormatsCount; ++i) {
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
