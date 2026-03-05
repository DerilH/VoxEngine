//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/Enums.h"
#include "VoxCore/containers/Containers.h"
#include "xxhash.h"

RENDER_NS
    class BlendState {
        friend struct std::hash<Vox::Render::BlendState>;
        uint8_t mColorWriteMask = 0;
        bool mblendEnable = false;
        BlendFactor mSrcFactor;
        BlendFactor mDstFactor;
        BlendFactor mSrcAlphaFactor;
        BlendFactor mDstAlphaFactor;
        BlendOp mColorBlendOp;
        BlendOp mAlphaBlendOp;

        BlendState() = default;

    public:

        uint8_t getColorWriteMask() const { return mColorWriteMask; }

        bool isBlendEnable() const { return mblendEnable; }

        BlendFactor getSrcFactor() const {return mSrcFactor;}

        BlendFactor getDstFactor() const {return mDstFactor;}

        BlendFactor getSrcAlphaFactor() const {return mSrcAlphaFactor;}

        BlendFactor getDstAlphaFactor() const {return mDstAlphaFactor;}

        BlendOp getColorBlendOp() const {return mColorBlendOp;}

        BlendOp getAlphaBlendOp() const {return mAlphaBlendOp;}


        class Builder {
            Vector<BlendState> mPerAttachment;
            uint32_t mCurrent = 0;
        public:

            Builder& startAttachment();

            inline Builder& endAttachment() {
                mCurrent++;
                return *this;
            }

            constexpr Builder& writes(bool r, bool g, bool b, bool a) {
                if (r) mPerAttachment[mCurrent].mColorWriteMask |= 0x00000001;
                if (g) mPerAttachment[mCurrent].mColorWriteMask |= 0x00000002;
                if (b) mPerAttachment[mCurrent].mColorWriteMask |= 0x00000004;
                if (a) mPerAttachment[mCurrent].mColorWriteMask |= 0x00000008;
                return *this;
            }

            BUILDER_ENTRY(Builder, enable, bool, mPerAttachment[mCurrent].mblendEnable);

            BUILDER_ENTRY(Builder, srcFactor, BlendFactor, mPerAttachment[mCurrent].mSrcFactor);

            BUILDER_ENTRY(Builder, srcAlphaFactor, BlendFactor, mPerAttachment[mCurrent].mSrcAlphaFactor);

            BUILDER_ENTRY(Builder, dstFactor, BlendFactor, mPerAttachment[mCurrent].mDstFactor);

            BUILDER_ENTRY(Builder, dstAlphaFactor, BlendFactor, mPerAttachment[mCurrent].mDstAlphaFactor);

            BUILDER_ENTRY(Builder, blendOp, BlendOp, mPerAttachment[mCurrent].mColorBlendOp);

            BUILDER_ENTRY(Builder, blendAlphaOp, BlendOp, mPerAttachment[mCurrent].mAlphaBlendOp);

            Vector<BlendState> build() const&;
        };

        inline static Builder GetBuilder() { return {}; }
    };
NS_END

namespace std {
    template<>
    struct hash<Vox::Render::BlendState> {
        inline size_t operator()(const Vox::Render::BlendState& s) const noexcept {
            XXH64_state_t* state = XXH64_createState();
            XXH64_reset(state, 0);

            XXH64_update(state, &s.mColorWriteMask, sizeof(uint8_t));
            XXH64_update(state, &s.mblendEnable, sizeof(bool));
            XXH64_update(state, &s.mSrcFactor, sizeof(Vox::Render::BlendFactor));
            XXH64_update(state, &s.mSrcAlphaFactor, sizeof(Vox::Render::BlendFactor));
            XXH64_update(state, &s.mDstFactor, sizeof(Vox::Render::BlendFactor));
            XXH64_update(state, &s.mDstAlphaFactor, sizeof(Vox::Render::BlendFactor));
            XXH64_update(state, &s.mColorBlendOp, sizeof(Vox::Render::BlendOp));
            XXH64_update(state, &s.mAlphaBlendOp, sizeof(Vox::Render::BlendOp));

            uint64_t h = XXH64_digest(state);
            XXH64_freeState(state);
            return h;
        }
    };
}