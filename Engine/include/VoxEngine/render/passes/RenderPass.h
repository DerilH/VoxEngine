//
// Created by deril on 2/13/26.
//

#pragma once

#include <VoxEngine/render/state/RenderingState.h>

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
        RenderingState mRenderingState;

        RenderPass(RenderPassType mType, ArrayView<AttachmentDesc> reads, ArrayView<AttachmentDesc> writes, bool isControlPass = false);
    public:
        virtual ~RenderPass() = default;

        const bool isControlPass;

        HashSet<RenderPassRef> mNext;
        HashSet<RenderPassRef> mPrev;

        virtual void execute(RenderContext context) = 0;

        RenderPassType getType() const {
            return mType;
        }

        ArrayView<AttachmentDesc> getWrites() const {
            return mWrites;
        }

        ArrayView<AttachmentDesc> getReads() const {
            return mReads;
        }

        void setExtent(Extent extent) {
            mExtent = extent;
        }

        ConstRef<RenderingState> getState() const {
            return &mRenderingState;
        }
    };

NS_END