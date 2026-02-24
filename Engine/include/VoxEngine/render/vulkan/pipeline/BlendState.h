//
// Created by deril on 2/24/26.
//

#pragma once


#include "VoxCore/containers/Containers.h"
#include "VoxCore/containers/Buffer.h"
VULKAN_NS
class BlendState {
    VkPipelineColorBlendStateCreateInfo mState;
public:
    BlendState(VkPipelineColorBlendStateCreateInfo state);

    inline operator const VkPipelineColorBlendStateCreateInfo& () const { return mState; }
    inline const VkPipelineColorBlendStateCreateInfo& getState() const { return mState; }

};

class BlendStateBuilder {
    Buffer <VkPipelineColorBlendAttachmentState> mPerAttachment;
    uint32_t mCurrent = 0;
public:
    BlendStateBuilder(uint32_t attachmentsCount);
    inline BlendStateBuilder& endAttachment() {mCurrent++; return *this;}

    constexpr BlendStateBuilder& writes(bool r, bool g, bool b, bool a){
        if (r) mPerAttachment[mCurrent].colorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
        if (g) mPerAttachment[mCurrent].colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
        if (b) mPerAttachment[mCurrent].colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
        if (a) mPerAttachment[mCurrent].colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
        return *this;
    }
    BUILDER_ENTRY(BlendStateBuilder, enable, VkBool32, mPerAttachment[mCurrent].blendEnable);
    BUILDER_ENTRY(BlendStateBuilder, srcFactor, VkBlendFactor, mPerAttachment[mCurrent].srcColorBlendFactor);
    BUILDER_ENTRY(BlendStateBuilder, srcAlphaFactor, VkBlendFactor, mPerAttachment[mCurrent].srcAlphaBlendFactor);
    BUILDER_ENTRY(BlendStateBuilder, dstFactor, VkBlendFactor, mPerAttachment[mCurrent].dstColorBlendFactor);
    BUILDER_ENTRY(BlendStateBuilder, dstAlphaFactor, VkBlendFactor, mPerAttachment[mCurrent].dstAlphaBlendFactor);
    BUILDER_ENTRY(BlendStateBuilder, blendOp, VkBlendOp, mPerAttachment[mCurrent].colorBlendOp);
    BUILDER_ENTRY(BlendStateBuilder, blendAlphaOp, VkBlendOp, mPerAttachment[mCurrent].alphaBlendOp);
    BlendState build() const &;
};
NS_END
