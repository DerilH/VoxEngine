//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/Enums.h"
#include "xxhash.h"

RENDER_NS
class RasterizerState {
    friend struct std::hash<Vox::Render::RasterizerState>;

    PolygonMode mPolygonMode;
    float mLineWidth;
    CullMode mCullMode;
    FrontFace mFrontFace;
    bool mDepthClampEnable;
    bool mDiscardEnable;
    bool mDepthBiasEnable;

    RasterizerState(PolygonMode mPolygonMode, float mLineWidth, CullMode mCullMode, FrontFace mFrontFace, bool mDepthClampEnable, bool mDiscardEnable, bool mDepthBiasEnable) : mPolygonMode(mPolygonMode), mLineWidth(mLineWidth), mCullMode(mCullMode), mFrontFace(mFrontFace), mDepthClampEnable(mDepthClampEnable), mDiscardEnable(mDiscardEnable), mDepthBiasEnable(mDepthBiasEnable) {}


public:

    inline PolygonMode getPolygonMode() const {return mPolygonMode;}
    inline float getLineWidth() const {return mLineWidth;}
    inline CullMode getCullMode() const {return mCullMode;}
    inline FrontFace getFrontFace() const {return mFrontFace;}
    inline bool getDepthClampEnable() const {return mDepthClampEnable;}
    inline bool getDiscardEnabled() const {return mDiscardEnable;}
    inline bool getDepthBiasEnabled() const {return mDepthBiasEnable;}

    class Builder;
    static Builder GetBuilder();

    class Builder {
        PolygonMode mPolygonMode;
        float mLineWidth;
        CullMode mCullMode;
        FrontFace mFrontFace;
        bool mDepthClampEnable;
        bool mDiscardEnable;
        bool mDepthBiasEnable;
    public:

        BUILDER_ENTRY(Builder, polygonMode, PolygonMode, mPolygonMode)
        BUILDER_ENTRY(Builder, lineWidth, float, mLineWidth)
        BUILDER_ENTRY(Builder, cullMode, CullMode, mCullMode)
        BUILDER_ENTRY(Builder, frontFace, FrontFace, mFrontFace)
        BUILDER_ENTRY(Builder, depthClamp, bool, mDepthClampEnable)
        BUILDER_ENTRY(Builder, rasterizerDiscard, bool, mDiscardEnable)
        BUILDER_ENTRY(Builder, depthBias, bool, mDepthBiasEnable)

        RasterizerState build() const &;
   };
};
NS_END

namespace std {
    template<>
    struct hash<Vox::Render::RasterizerState> {
        inline size_t operator()(const Vox::Render::RasterizerState& s) const noexcept {

           XXH64_state_t* state = XXH64_createState();
           XXH64_reset(state, 0);
           XXH64_update(state, &s.mPolygonMode, sizeof(Vox::Render::PolygonMode));
           XXH64_update(state, &s.mLineWidth, sizeof(float));
           XXH64_update(state, &s.mCullMode, sizeof(Vox::Render::CullMode));
           XXH64_update(state, &s.mFrontFace, sizeof(Vox::Render::FrontFace));
           XXH64_update(state, &s.mDepthClampEnable, sizeof(bool));
           XXH64_update(state, &s.mDiscardEnable, sizeof(bool));
           XXH64_update(state, &s.mDepthBiasEnable, sizeof(bool));

           uint64_t h = XXH64_digest(state);
           XXH64_freeState(state);
           return h;
        }
    };
}