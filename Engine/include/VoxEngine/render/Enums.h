//
// Created by deril on 3/1/26.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include "VoxCore/Define.h"
#include "VoxCore/Assert.h"
#include "VoxCore/Logger.h"



#define FRONT_FACE_LIST(M)\
    M(FrontFace, CLOCKWISE,    VK_FRONT_FACE_CLOCKWISE,       NO, NO)\
    M(FrontFace, COUNTER_CLOCKWISE,   VK_FRONT_FACE_COUNTER_CLOCKWISE,      NO, NO)\

#define FRONT_FACE_LIST(M)\
    M(FrontFace, CLOCKWISE,    VK_FRONT_FACE_CLOCKWISE,       NO, NO)\
    M(FrontFace, COUNTER_CLOCKWISE,   VK_FRONT_FACE_COUNTER_CLOCKWISE,      NO, NO)\

#define CULL_MODE_LIST(M)\
    M(CullMode, NONE,    VK_CULL_MODE_NONE,       NO, NO)\
    M(CullMode, BACK,   VK_CULL_MODE_BACK_BIT,      NO, NO)\
    M(CullMode, FRONT,   VK_CULL_MODE_FRONT_BIT,      NO, NO)\
    M(CullMode, FRONT_BACK,   VK_CULL_MODE_FRONT_AND_BACK,      NO, NO)\

#define POLYGON_MODE_LIST(M)\
    M(PolygonMode, FILL,    VK_POLYGON_MODE_FILL,       NO, NO)\
    M(PolygonMode, LINE,   VK_POLYGON_MODE_LINE,      NO, NO)\
    M(PolygonMode, POINT,   VK_POLYGON_MODE_POINT,      NO, NO)\

#define PRIMITIVE_TOPOLOGY_LIST(M)\
    M(PrimitiveTopology, TRIANGLE_LIST,    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,       NO, NO)\
    M(PrimitiveTopology, TRIANGLE_STRIP,   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,      NO, NO)\
    M(PrimitiveTopology, TRIANGLE_FAN,   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,      NO, NO)\

#define BUFFER_USAGE_LIST(M)\
    M(BufferUsage, INDEX,    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,       NO, NO)\
    M(BufferUsage, VERTEX,   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,      NO, NO)\
    M(BufferUsage, SSBO,   VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,      NO, NO)\
    M(BufferUsage, UNIFORM,   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,      NO, NO)\

#define INDEX_TYPE_LIST(M)\
    M(IndexType, UINT8,    VK_INDEX_TYPE_UINT8,       NO, NO)\
    M(IndexType, UINT16,   VK_INDEX_TYPE_UINT16,      NO, NO)\
    M(IndexType, UINT32,   VK_INDEX_TYPE_UINT32,      NO, NO)\

#define FORMAT_LIST(M)\
    M(Format, RGBA8, VK_FORMAT_R8G8B8A8_UNORM,  NO, NO)\
    M(Format, D32,   VK_FORMAT_D32_SFLOAT,      NO, NO)\

#define SHADER_STAGE_LIST(M)\
    M(ShaderStage, VERTEX_SHADER,     VK_SHADER_STAGE_VERTEX_BIT,   NO, NO)\
    M(ShaderStage, FRAGMENT_SAHDER,   VK_SHADER_STAGE_FRAGMENT_BIT, NO, NO)\

#define MSAA_COUNT_LIST(M)\
    M(MSAASamples, COUNT_1,     VK_SAMPLE_COUNT_1_BIT,   NO, NO)\
    M(MSAASamples, COUNT_2,   VK_SAMPLE_COUNT_2_BIT, NO, NO)\
    M(MSAASamples, COUNT_4,   VK_SAMPLE_COUNT_4_BIT, NO, NO)\
    M(MSAASamples, COUNT_8,   VK_SAMPLE_COUNT_8_BIT, NO, NO)\
    M(MSAASamples, COUNT_16,   VK_SAMPLE_COUNT_16_BIT, NO, NO)\
    M(MSAASamples, COUNT_32,   VK_SAMPLE_COUNT_32_BIT, NO, NO)\
    M(MSAASamples, COUNT_64,   VK_SAMPLE_COUNT_64_BIT, NO, NO)\

#define BLEND_OP_LIST(M)\
    M(BlendOp, ADD, VK_BLEND_OP_ADD, NO, NO)\
    M(BlendOp, SUBTRACT, VK_BLEND_OP_SUBTRACT, NO, NO)\
    M(BlendOp, REVERSE_SUBTRACT, VK_BLEND_OP_REVERSE_SUBTRACT, NO, NO)\
    M(BlendOp, MIN, VK_BLEND_OP_MIN, NO, NO)\
    M(BlendOp, MAX, VK_BLEND_OP_MAX, NO, NO)\
    M(BlendOp, MULTIPLY, VK_BLEND_OP_MULTIPLY_EXT, NO, NO)\
    M(BlendOp, SCREEN, VK_BLEND_OP_SCREEN_EXT, NO, NO)\
    M(BlendOp, OVERLAY, VK_BLEND_OP_OVERLAY_EXT, NO, NO)\

#define BLEND_FACTOR_LIST(M)\
    M(BlendFactor, ZERO, VK_BLEND_FACTOR_ZERO, NO, NO)\
    M(BlendFactor, ONE, VK_BLEND_FACTOR_ONE, NO, NO)\
    M(BlendFactor, SRC_COLOR, VK_BLEND_FACTOR_SRC_COLOR, NO, NO)\
    M(BlendFactor, ONE_MINUS_SRC_COLOR, VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR, NO, NO)\
    M(BlendFactor, DST_COLOR, VK_BLEND_FACTOR_DST_COLOR, NO, NO)\
    M(BlendFactor, ONE_MINUS_DST_COLOR, VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR, NO, NO)\
    M(BlendFactor, SRC_ALPHA, VK_BLEND_FACTOR_SRC_ALPHA, NO, NO)\
    M(BlendFactor, ONE_MINUS_SRC_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA, NO, NO)\
    M(BlendFactor, DST_ALPHA, VK_BLEND_FACTOR_DST_ALPHA, NO, NO)\
    M(BlendFactor, ONE_MINUS_DST_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA, NO, NO)\
    M(BlendFactor, CONSTANT_COLOR, VK_BLEND_FACTOR_CONSTANT_COLOR, NO, NO)\
    M(BlendFactor, ONE_MINUS_CONSTANT_COLOR, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR, NO, NO)\
    M(BlendFactor, CONSTANT_ALPHA, VK_BLEND_FACTOR_CONSTANT_ALPHA, NO, NO)\
    M(BlendFactor, ONE_MINUS_CONSTANT_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA, NO, NO)\
    M(BlendFactor, SRC_ALPHA_SATURATE, VK_BLEND_FACTOR_SRC_ALPHA_SATURATE, NO, NO)\
    M(BlendFactor, SRC1_COLOR, VK_BLEND_FACTOR_SRC1_COLOR, NO, NO)\
    M(BlendFactor, ONE_MINUS_SRC1_COLOR, VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR, NO, NO)\
    M(BlendFactor, SRC1_ALPHA, VK_BLEND_FACTOR_SRC1_ALPHA, NO, NO)\
    M(BlendFactor, ONE_MINUS_SRC1_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA, NO, NO)\


#define DEFINE_CROSS_ENUM(enumName, LIST_MACRO) DEFINE_CROSS_ENUM_IMPL(enumName, LIST_MACRO, CROSS_EXPAND_NAME)

#define DEFINE_CROSS_ENUM_IMPL(enumName, LIST_MACRO, EXPAND_MACRO) \
    enum class enumName {\
        LIST_MACRO(EXPAND_MACRO)\
    };\

#define CROSS_EXPAND_NAME(enumType, name, vk, dx, gl) name,

#define CROSS_EXPAND_VK_CASE(enumType, name, vk, dx, gl) case enumType::name: return vk;
#define CROSS_EXPAND_DX_CASE(enumType, name, vk, dx, gl) case enumType::name: return dx;
#define CROSS_EXPAND_GL_CASE(enumType, name, vk, dx, gl) case enumType::name: return gl;

#define CROSS_EXPAND_VK_CASE_FROM(enumType, name, vk, dx, gl) case vk: return enumType::name;
#define CROSS_EXPAND_DX_CASE_FROM(enumType, name, vk, dx, gl) case dx: return enumType::name;
#define CROSS_EXPAND_GL_CASE_FROM(enumType, name, vk, dx, gl) case gl: return enumType::name;

#define DEFINE_ENUM_CONVERTER(EnumType, ApiEnumType, LIST_MACRO, EXPAND_MACRO, Name)\
constexpr ApiEnumType to##Name(EnumType format) {\
    switch (format) {\
        LIST_MACRO(EXPAND_MACRO)\
        default:\
            LOG_ERROR("{}", (int)format);\
            VOX_ASSERT(false, "Enum not implemented");\
    };\
}\

#define DEFINE_ENUM_CONVERTER_FROM(EnumType, ApiEnumType, LIST_MACRO, EXPAND_MACRO, Name)\
constexpr EnumType from##Name(ApiEnumType format) {\
    switch (format) {\
        LIST_MACRO(EXPAND_MACRO)\
        default:\
            LOG_ERROR("{}", (int)format);\
            VOX_ASSERT(false, "Enum not implemented");\
    }\
    __builtin_unreachable();\
}

#define DEFINE_ENUM_CONVERTER_VK(EnumType, ApiEnumType, LIST_MACRO) DEFINE_ENUM_CONVERTER(EnumType, ApiEnumType, LIST_MACRO, CROSS_EXPAND_VK_CASE, Vk)
#define DEFINE_ENUM_CONVERTER_FROM_VK(EnumType, ApiEnumType, LIST_MACRO) DEFINE_ENUM_CONVERTER_FROM(EnumType, ApiEnumType, LIST_MACRO, CROSS_EXPAND_VK_CASE_FROM, Vk)

#define DEFINE_ENUM_CONVERTER_DX(EnumType, ApiEnumType, LIST_MACRO) DEFINE_ENUM_CONVERTER(EnumType, ApiEnumType, LIST_MACRO, CROSS_EXPAND_DX_CASE, Vk)
#define DEFINE_ENUM_CONVERTER_FROM_DX(EnumType, ApiEnumType, LIST_MACRO) DEFINE_ENUM_CONVERTER_FROM(EnumType, ApiEnumType, LIST_MACRO, CROSS_EXPAND_DX_CASE_FROM, Vk)

#define DEFINE_ENUM_CONVERTER_GL(EnumType, ApiEnumType, LIST_MACRO) DEFINE_ENUM_CONVERTER(EnumType, ApiEnumType, LIST_MACRO, CROSS_EXPAND_GL_CASE, Vk)
#define DEFINE_ENUM_CONVERTER_FROM_GL(EnumType, ApiEnumType, LIST_MACRO) DEFINE_ENUM_CONVERTER_FROM(EnumType, ApiEnumType, LIST_MACRO, CROSS_EXPAND_GL_CASE_FROM, Vk)


RENDER_NS
    DEFINE_CROSS_ENUM(Format, FORMAT_LIST);
    DEFINE_CROSS_ENUM(ShaderStage, SHADER_STAGE_LIST);
    DEFINE_CROSS_ENUM(IndexType, INDEX_TYPE_LIST);
    DEFINE_CROSS_ENUM(BufferUsage, BUFFER_USAGE_LIST);
    DEFINE_CROSS_ENUM(PrimitiveTopology, PRIMITIVE_TOPOLOGY_LIST);
    DEFINE_CROSS_ENUM(PolygonMode, POLYGON_MODE_LIST);
    DEFINE_CROSS_ENUM(CullMode, CULL_MODE_LIST);
    DEFINE_CROSS_ENUM(FrontFace, FRONT_FACE_LIST);
    DEFINE_CROSS_ENUM(BlendFactor, BLEND_FACTOR_LIST);
    DEFINE_CROSS_ENUM(BlendOp, BLEND_OP_LIST)
    DEFINE_CROSS_ENUM(MSAASamples, MSAA_COUNT_LIST);
#include "IndexType.inl"
NS_END
