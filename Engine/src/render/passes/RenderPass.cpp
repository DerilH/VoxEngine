//
// Created by deril on 2/13/26.
//

#include "VoxEngine/render/RenderPassType.h"
#include "VoxEngine/render/passes/RenderPass.h"

RENDER_NS
    RenderingState createState(const ArrayView<AttachmentDesc>& writes) {
        Format formats[8];
        for (int i = 0; i < writes.size(); i++) {
            formats[i] = writes[i].texture->getFormat();
        }
        return RenderingState(ArrayView(&formats[0], writes.size()), Format::UNDEFINED, Format::UNDEFINED);
    }

    RenderPass::RenderPass(RenderPassType mType, ArrayView<AttachmentDesc> reads, ArrayView<AttachmentDesc> writes, bool isControlPass) : mType(mType), mWrites(writes), mReads(reads), mRenderingState(createState(writes)), isControlPass(isControlPass) {
    }

NS_END
