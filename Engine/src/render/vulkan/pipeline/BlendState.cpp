//
// Created by deril on 2/24/26.
//

#include "VoxEngine/render/state/BlendState.h"


RENDER_NS
    Vector<BlendState> BlendState::Builder::build() const& {
        return mPerAttachment;
    }

    BlendState::Builder& BlendState::Builder::startAttachment() {
        mPerAttachment.push_back({});
        return *this;
    }
NS_END




