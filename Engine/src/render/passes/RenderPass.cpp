//
// Created by deril on 2/13/26.
//

#include <stdexcept>
#include "VoxEngine/render/RenderPassType.h"
#include "VoxEngine/render/passes/RenderPass.h"

RENDER_NS
    RenderPassType RenderPass::getType() const {
        return mType;
    }

    RenderPass::RenderPass(RenderPassType mType, ArrayView<AttachmentDesc> reads, ArrayView<AttachmentDesc> writes) : mType(mType), mReads(reads), mWrites(writes) {
    }

    ArrayView<AttachmentDesc>& RenderPass::getWrites() {
        return mWrites;
    }

    ArrayView<AttachmentDesc>& RenderPass::getReads() {
        return mReads;
    }

    void RenderPass::setExtent(Extent extent) {
        mExtent = extent;
    }
NS_END

