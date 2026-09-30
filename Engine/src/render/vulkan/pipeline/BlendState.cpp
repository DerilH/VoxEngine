//
// Created by deril on 2/24/26.
//

#include "VoxEngine/render/state/BlendState.h"
#include "VoxEngine/render/vulkan/pipeline/BlendState.h"


RENDER_NS
    Vector<BlendState> BlendState::Builder::build() const& {
        VOX_ASSERT(mPerAttachment.size() > 0, "No blend attachments provided");
        return mPerAttachment;
    }

    BlendState::Builder& BlendState::Builder::startAttachment() {
        mPerAttachment.push_back({});
        return *this;
    }
NS_END



