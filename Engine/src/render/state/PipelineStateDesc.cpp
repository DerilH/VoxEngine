//
// Created by deril on 3/4/26.
//

#include "VoxEngine/render/state/PipelineStateDesc.h"

RENDER_NS
size_t PipelineStateDesc::hash() const {
    if(mCachedHash != 0) return mCachedHash;

    XXH64_state_t* state = XXH64_createState();
    XXH64_reset(state, 0);

    uint64_t hash = std::hash<Vox::Render::BlendState>{}(mBlend);
    XXH64_update(state, &hash, sizeof(uint64_t));
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
    return h;
}
NS_END