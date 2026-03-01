//
// Created by deril on 3/1/26.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include "VoxCore/Define.h"
#include "VoxEngine/render/Enums.h"

RENDER_NS
    DEFINE_ENUM_CONVERTER(Format, VkFormat, FORMAT_LIST, CROSS_EXPAND_VK);
    DEFINE_ENUM_CONVERTER(ShaderStage, VkShaderStageFlagBits, SHADER_STAGE_LIST, CROSS_EXPAND_VK);
NS_END