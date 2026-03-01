//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxCore/Define.h"


#define FORMAT_LIST(M) \
    M(RGBA8, VK_FORMAT_R8G8B8A8_UNORM,  NO) \
    M(D32,   VK_FORMAT_D32_SFLOAT,      NO)     \

#define SHADER_STAGE_LIST(M) \
    M(VERTEX_SHADER,     VK_SHADER_STAGE_VERTEX_BIT,   NO) \
    M(FRAGMENT_SAHDER,   VK_SHADER_STAGE_FRAGMENT_BIT, NO)     \


#define DEFINE_CROSS_ENUM(enumName, LIST_MACRO, EXPAND_MACRO) \
    enum class enumName {                                          \
        LIST_MACRO(EXPAND_MACRO)                                   \
    };

#define CROSS_EXPAND_NAME(name, vk, dx) name,

#define CROSS_EXPAND_VK(name, vk, dx) name: return vk;
#define CROSS_EXPAND_DX(name, vk, dx) name: return dx;

#define DEFINE_ENUM_CONVERTER(EnumType, ApiEnumType, LIST_MACRO, EXPAND_MACRO)        \
constexpr ApiEnumType toVk(EnumType format) {                           \
    switch (format) {                       \
        LIST_MACRO(case EnumType::EXPAND_MACRO) \
    }                                                                   \
    __builtin_unreachable();                \
}

RENDER_NS
    DEFINE_CROSS_ENUM(Format, FORMAT_LIST, CROSS_EXPAND_NAME);
    DEFINE_CROSS_ENUM(ShaderStage, SHADER_STAGE_LIST, CROSS_EXPAND_NAME);
NS_END
