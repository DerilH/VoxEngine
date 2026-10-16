#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/render/Enums.h"
#include <xxh3.h>

RENDER_NS
class DepthState {
    friend struct std::hash<Vox::Render::DepthState>;

    bool mDepthTestEnable = true;
    bool mDepthWriteEnable = true;
    CompareOp mDepthCompareOp = CompareOp::LESS_OR_EQUAL;

public:
    explicit DepthState(bool depthTest = true, bool depthWrite = true, CompareOp compareOp = CompareOp::LESS_OR_EQUAL)
        : mDepthTestEnable(depthTest), mDepthWriteEnable(depthWrite), mDepthCompareOp(compareOp) {}

    inline bool isDepthTestEnable() const { return mDepthTestEnable; }
    inline bool isDepthWriteEnable() const { return mDepthWriteEnable; }
    inline CompareOp getDepthCompareOp() const { return mDepthCompareOp; }

    inline bool operator==(const DepthState& other) const {
        return mDepthTestEnable == other.mDepthTestEnable &&
               mDepthWriteEnable == other.mDepthWriteEnable &&
               mDepthCompareOp == other.mDepthCompareOp;
    }

    class Builder;
    static Builder GetBuilder();

    class Builder {
        bool mDepthTestEnable = true;
        bool mDepthWriteEnable = true;
        CompareOp mDepthCompareOp = CompareOp::LESS_OR_EQUAL;
    public:
        BUILDER_ENTRY(Builder, depthTest, bool, mDepthTestEnable)
        BUILDER_ENTRY(Builder, depthWrite, bool, mDepthWriteEnable)
        BUILDER_ENTRY(Builder, compareOp, CompareOp, mDepthCompareOp)

        DepthState build() const & {
            return DepthState(mDepthTestEnable, mDepthWriteEnable, mDepthCompareOp);
        }
    };
};

inline DepthState::Builder DepthState::GetBuilder() {
    return Builder();
}

NS_END

namespace std {
    template<>
    struct hash<Vox::Render::DepthState> {
        inline size_t operator()(const Vox::Render::DepthState& s) const noexcept {
            XXH64_state_t* state = XXH64_createState();
            XXH64_reset(state, 0);
            XXH64_update(state, &s.mDepthTestEnable, sizeof(bool));
            XXH64_update(state, &s.mDepthWriteEnable, sizeof(bool));
            XXH64_update(state, &s.mDepthCompareOp, sizeof(Vox::Render::CompareOp));
            uint64_t h = XXH64_digest(state);
            XXH64_freeState(state);
            return h;
        }
    };
}
