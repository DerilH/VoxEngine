//
// Created by deril on 2/25/26.
//

#pragma once


#include "RenderPass.h"

VULKAN_NS
class PresentPass : public RenderPass {

public:
    explicit PresentPass(RenderPassType mType, const Vector<AttachmentDesc> &reads, const Vector<AttachmentDesc> &writes);
    void execute(const RenderContext& context) override;
};
NS_END
