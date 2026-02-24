//
// Created by deril on 2/24/26.
//

#include "VoxEngine/render/vulkan/pipeline/BlendState.h"

VULKAN_NS
    BlendState::BlendState(VkPipelineColorBlendStateCreateInfo state) : mState(state) {

    }

    BlendState BlendStateBuilder::build() const &{
        VOX_ASSERT(mPerAttachment.size > 0, "Invalid attachments count");
        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = mPerAttachment.pData;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        return {colorBlending};
    }

    BlendStateBuilder::BlendStateBuilder(uint32_t attachmentsCount) : mPerAttachment(new VkPipelineColorBlendAttachmentState[attachmentsCount](), attachmentsCount){
    }
NS_END


