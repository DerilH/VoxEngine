//
// Created by deril on 2/25/26.
//

#pragma once


#include "RenderPass.h"
#include "VoxEngine/render/RenderContext.h"

RENDER_NS
class GeometryPass : public RenderPass {

public:
    explicit GeometryPass(RenderPassType mType, const ArrayView<AttachmentDesc>& reads, const ArrayView<AttachmentDesc>& writes);

    void execute(RenderContext context) override;
};
NS_END
