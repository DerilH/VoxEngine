//
// Created by deril on 3/4/26.
//

#include "VoxEngine/render/state/PipelineStateDesc.h"
#include <xxh3.h>

RENDER_NS
size_t PipelineStateDesc::hash() const {
    if(mCachedHash != 0) return mCachedHash;

    XXH64_state_t* state = XXH64_createState();
    XXH64_reset(state, 0);

    uint64_t hash;
    for (int i = 0; i < mBlend.size(); ++i) {
        hash = std::hash<Vox::Render::BlendState>{}(mBlend[i]);
        XXH64_update(state, &hash, sizeof(uint64_t));
    }
    hash = std::hash<Vox::Render::MSAAState>{}(mMsaa);
    XXH64_update(state, &hash, sizeof(uint64_t));
    hash = std::hash<Vox::Render::RasterizerState>{}(mRasterizer);
    XXH64_update(state, &hash, sizeof(uint64_t));
    hash = std::hash<Vox::Render::ShaderState>{}(mShader);
    XXH64_update(state, &hash, sizeof(uint64_t));
    hash = std::hash<Vox::Render::RenderingState>{}(mRenderingState);
    XXH64_update(state, &hash, sizeof(uint64_t));

    XXH64_update(state, &mTopology, sizeof(PrimitiveTopology));
    uint64_t h = XXH64_digest(state);
    XXH64_freeState(state);
    mCachedHash = h;
    return h;
}

    PipelineStateDesc::PipelineStateDesc(const PipelineStateDesc& desc) : mCachedHash(desc.mCachedHash), mBlend(desc.mBlend), mMsaa(desc.mMsaa), mRasterizer(desc.mRasterizer), mShader(desc.mShader), mRenderingState(desc.mRenderingState), mTopology(desc.mTopology) {

    }

    bool PipelineStateDesc::operator==(const PipelineStateDesc& other) const {
        bool same = mMsaa == other.mMsaa;
        same = same && mRenderingState == other.mRenderingState;
        same = same && mShader == other.mShader;
        same = same && mRasterizer == other.mRasterizer;
        same = same && mTopology == other.mTopology;

        for(int i = 0; i < mBlend.size(); ++i) {
            same = same && mBlend[i] == other.mBlend[i];
        }
        return same;
    }
NS_END