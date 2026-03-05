//
// Created by deril on 3/4/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/Enums.h"

RENDER_NS
class MSAAState {
    friend struct std::hash<Vox::Render::MSAAState>;

    MSAASamples mSamples = MSAASamples::COUNT_1;
    bool mEnableSampleShading = false;
public:
    explicit MSAAState(MSAASamples samples, bool enableSampleShading) : mSamples(samples), mEnableSampleShading(enableSampleShading) {}
    inline MSAASamples getSamples() const { return mSamples; }
    inline bool isSampleShading() const { return mEnableSampleShading; }
};
NS_END
namespace std {
    template<>
    struct hash<Vox::Render::MSAAState> {
        inline size_t operator()(const Vox::Render::MSAAState& s) const noexcept {
            XXH64_state_t* state = XXH64_createState();
            XXH64_reset(state, 0);
            XXH64_update(state, &s.mSamples, sizeof(Vox::Render::MSAASamples));
            XXH64_update(state, &s.mEnableSampleShading, sizeof(bool));
            uint64_t h = XXH64_digest(state);
            XXH64_freeState(state);
            return h;
        }
    };
}