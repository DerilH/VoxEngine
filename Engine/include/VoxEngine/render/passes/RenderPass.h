//
// Created by deril on 2/13/26.
//

#pragma once

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

        RenderPass(RenderPassType mType, ArrayView<AttachmentDesc> reads, ArrayView<AttachmentDesc> writes, bool isControlPass = false);

    public:
        const bool isControlPass;

        HashSet<RenderPassRef> mNext;
        HashSet<RenderPassRef> mPrev;

        virtual void execute(RenderContext context) = 0;

        RenderPassType getType() const;

        ArrayView<AttachmentDesc>& getWrites();

        ArrayView<AttachmentDesc>& getReads();

        void setExtent(Extent extent);
    };

NS_END