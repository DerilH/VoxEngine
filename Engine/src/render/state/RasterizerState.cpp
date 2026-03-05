//
// Created by deril on 3/4/26.
//
#include "VoxEngine/render/state/RasterizerState.h"

RENDER_NS
    RasterizerState RasterizerState::Builder::build() const& {
        return {mPolygonMode, mLineWidth, mCullMode, mFrontFace, mDepthClampEnable, mDiscardEnable, mDepthBiasEnable};
    }
    RasterizerState::Builder RasterizerState::GetBuilder() {
        return {};
    }
NS_END

