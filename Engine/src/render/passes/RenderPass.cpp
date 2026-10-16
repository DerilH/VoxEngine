//
// Created by deril on 2/13/26.
//

#include "VoxEngine/render/RenderPassType.h"
#include "VoxEngine/render/passes/RenderPass.h"

RENDER_NS
    RenderingState createState(const ArrayView<AttachmentDesc>& writes) {
        Format colorFormats[8];
        for (int i = 0; i < 8; ++i) colorFormats[i] = Format::UNDEFINED;
        uint8_t colorCount = 0;
        Format depthFormat = Format::UNDEFINED;
        Format stencilFormat = Format::UNDEFINED;

        for (int i = 0; i < writes.size(); i++) {
            Format fmt = writes[i].texture->getFormat();
            if (isDepthFormat(fmt)) {
                depthFormat = fmt;
                if (hasStencilComponent(fmt)) {
                    stencilFormat = fmt;
                }
            } else {
                if (colorCount < 8) {
                    colorFormats[colorCount++] = fmt;
                }
            }
        }
        return RenderingState(ArrayView(&colorFormats[0], colorCount), depthFormat, stencilFormat);
    }

    RenderPass::RenderPass(RenderPassType mType, ArrayView<AttachmentDesc> reads, ArrayView<AttachmentDesc> writes, bool isControlPass) : mType(mType), mWrites(writes), mReads(reads), mRenderingState(createState(writes)), isControlPass(isControlPass) {
    }

NS_END
