//
// Created by deril on 2/25/26.
//

#pragma once


#include "RenderPass.h"
#include "VoxEngine/render/RenderContext.h"

VULKAN_NS
class GeometryPass : public RenderPass {

public:
    GeometryPass(RenderPassType mType, const Vector<AttachmentDesc>& reads, const Vector<AttachmentDesc>& writes);

    void execute(const RenderContext& context) override;
};
NS_END
