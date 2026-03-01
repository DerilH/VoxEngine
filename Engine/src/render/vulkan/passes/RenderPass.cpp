//
// Created by deril on 2/13/26.
//

#include <stdexcept>
#include "VoxEngine/render/vulkan/LogicalDevice.h"
#include "VoxEngine/render/vulkan/passes/RenderPass.h"

namespace Vox::Render::Vulkan {
    void end(const CommandBuffer& cmd) {
        vkCmdEndRendering(cmd);
    }

    RenderPassType RenderPass::getType() const {
        return mType;
    }

    void RenderPass::setCmdBuffer(CommandBuffer* buffer) {
        mCmdBuffer = buffer;
    }

    RenderPass::RenderPass(RenderPassType mType, Vector<AttachmentDesc> reads, Vector<AttachmentDesc> writes) : mType(mType), mReads(std::move(reads)), mWrites(std::move(writes)) {
    }

    Vector<AttachmentDesc>& RenderPass::getWrites() {
        return mWrites;
    }

    Vector<AttachmentDesc>& RenderPass::getReads() {
        return mReads;
    }

    void RenderPass::setExtent(Extent extent) {
        mExtent = extent;
    }
}

