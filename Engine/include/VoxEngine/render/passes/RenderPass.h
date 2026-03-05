//
// Created by deril on 2/13/26.
//

#pragma once

#include <cstddef>
#include <vulkan/vulkan_core.h>
#include <functional>
#include "VoxEngine/render/RenderPassType.h"

#include "VoxEngine/render/RenderTarget.h"
#include "VoxEngine/render/AttachmentDesc.h"
#include "VoxEngine/render/RenderContext.h"
#include "VoxCore/containers/ArrayView.h"

RENDER_NS
    class RenderPass {
    protected:
        const RenderPassType mType;
        ArrayView<AttachmentDesc> mWrites;
        ArrayView<AttachmentDesc> mReads;
        Extent mExtent;

        RenderPass(RenderPassType mType, ArrayView<AttachmentDesc> reads, ArrayView<AttachmentDesc> writes);

    public:
        HashSet<RenderPassRef> mNext;
        HashSet<RenderPassRef> mPrev;

        virtual void execute(Render::CommandBufferRef cmdBuffer) = 0;

        RenderPassType getType() const;

        ArrayView<AttachmentDesc>& getWrites();

        ArrayView<AttachmentDesc>& getReads();

        void setExtent(Extent extent);
    };

NS_END