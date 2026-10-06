//
// Created by deril on 2/28/26.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include "VoxCore/math/Extent.h"
#include "VoxCore/math/Extent3D.h"
#include "VoxCore/Define.h"
#include "VoxCore/render/Enums.h"
#include "VoxCore/Logger.h"

VULKAN_NS
#define VK_FORMAT_MAPPING_LIST(M)\
    M(Format, UNDEFINED,          VK_FORMAT_UNDEFINED)\
    M(Format, RGBA8,              VK_FORMAT_R8G8B8A8_UNORM)\
    M(Format, BGRA8,              VK_FORMAT_B8G8R8A8_UNORM)\
    M(Format, RGBA8_SRGB,         VK_FORMAT_R8G8B8A8_SRGB)\
    M(Format, BGRA8_SRGB,         VK_FORMAT_B8G8R8A8_SRGB)\
    M(Format, RGB10A2,            VK_FORMAT_A2B10G10R10_UNORM_PACK32)\
    M(Format, RG11B10F,           VK_FORMAT_B10G11R11_UFLOAT_PACK32)\
    M(Format, RG16F,              VK_FORMAT_R16G16_SFLOAT)\
    M(Format, RGBA16F,            VK_FORMAT_R16G16B16A16_SFLOAT)\
    M(Format, RGBA32F,            VK_FORMAT_R32G32B32A32_SFLOAT)\
    M(Format, R8,                 VK_FORMAT_R8_UNORM)\
    M(Format, R8_SRGB,            VK_FORMAT_R8_SRGB)\
    M(Format, R16F,               VK_FORMAT_R16_SFLOAT)\
    M(Format, D16,                VK_FORMAT_D16_UNORM)\
    M(Format, D32,                VK_FORMAT_D32_SFLOAT)\
    M(Format, D24S8,              VK_FORMAT_D24_UNORM_S8_UINT)\
    M(Format, D32S8,              VK_FORMAT_D32_SFLOAT_S8_UINT)\
    M(Format, BC1_RGB,            VK_FORMAT_BC1_RGB_UNORM_BLOCK)\
    M(Format, BC1_RGB_SRGB,       VK_FORMAT_BC1_RGB_SRGB_BLOCK)\
    M(Format, BC1_RGBA,           VK_FORMAT_BC1_RGBA_UNORM_BLOCK)\
    M(Format, BC1_RGBA_SRGB,      VK_FORMAT_BC1_RGBA_SRGB_BLOCK)\
    M(Format, BC3_RGBA,           VK_FORMAT_BC3_UNORM_BLOCK)\
    M(Format, BC3_RGBA_SRGB,      VK_FORMAT_BC3_SRGB_BLOCK)\
    M(Format, BC4_R,              VK_FORMAT_BC4_UNORM_BLOCK)\
    M(Format, BC5_RG,             VK_FORMAT_BC5_UNORM_BLOCK)\
    M(Format, BC7_RGBA,           VK_FORMAT_BC7_UNORM_BLOCK)\
    M(Format, BC7_RGBA_SRGB,      VK_FORMAT_BC7_SRGB_BLOCK)\
    M(Format, ETC2_RGB,           VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK)\
    M(Format, ETC2_RGB_SRGB,      VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK)\
    M(Format, ETC2_RGBA1,         VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK)\
    M(Format, ETC2_RGBA1_SRGB,    VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK)\
    M(Format, ETC2_RGBA8,         VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK)\
    M(Format, ETC2_RGBA8_SRGB,    VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK)\
    M(Format, ASTC_4x4,           VK_FORMAT_ASTC_4x4_UNORM_BLOCK)\
    M(Format, ASTC_4x4_SRGB,      VK_FORMAT_ASTC_4x4_SRGB_BLOCK)

#define VK_CULL_MODE_MAPPING_LIST(M)\
    M(CullMode, NONE,       VK_CULL_MODE_NONE)\
    M(CullMode, BACK,       VK_CULL_MODE_BACK_BIT)\
    M(CullMode, FRONT,      VK_CULL_MODE_FRONT_BIT)\
    M(CullMode, FRONT_BACK, VK_CULL_MODE_FRONT_AND_BACK)

#define VK_FRONT_FACE_MAPPING_LIST(M)\
    M(FrontFace, CLOCKWISE,         VK_FRONT_FACE_CLOCKWISE)\
    M(FrontFace, COUNTER_CLOCKWISE, VK_FRONT_FACE_COUNTER_CLOCKWISE)

#define VK_POLYGON_MODE_MAPPING_LIST(M)\
    M(PolygonMode, FILL,  VK_POLYGON_MODE_FILL)\
    M(PolygonMode, LINE,  VK_POLYGON_MODE_LINE)\
    M(PolygonMode, POINT, VK_POLYGON_MODE_POINT)

#define VK_PRIMITIVE_TOPOLOGY_MAPPING_LIST(M)\
    M(PrimitiveTopology, POINT_LIST,     VK_PRIMITIVE_TOPOLOGY_POINT_LIST)\
    M(PrimitiveTopology, LINE_LIST,      VK_PRIMITIVE_TOPOLOGY_LINE_LIST)\
    M(PrimitiveTopology, LINE_STRIP,     VK_PRIMITIVE_TOPOLOGY_LINE_STRIP)\
    M(PrimitiveTopology, TRIANGLE_LIST,  VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST)\
    M(PrimitiveTopology, TRIANGLE_STRIP, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP)\
    M(PrimitiveTopology, TRIANGLE_FAN,   VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN)

#define VK_BUFFER_USAGE_MAPPING_LIST(M)\
    M(BufferUsage, INDEX,        VK_BUFFER_USAGE_INDEX_BUFFER_BIT)\
    M(BufferUsage, VERTEX,       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT)\
    M(BufferUsage, SSBO,         VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)\
    M(BufferUsage, UNIFORM,      VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)\
    M(BufferUsage, TRANSFER_SRC, VK_BUFFER_USAGE_TRANSFER_SRC_BIT)

#define VK_INDEX_TYPE_MAPPING_LIST(M)\
    M(IndexType, UINT8,  VK_INDEX_TYPE_UINT8)\
    M(IndexType, UINT16, VK_INDEX_TYPE_UINT16)\
    M(IndexType, UINT32, VK_INDEX_TYPE_UINT32)

#define VK_SHADER_STAGE_MAPPING_LIST(M)\
    M(ShaderStage, VERTEX,   VK_SHADER_STAGE_VERTEX_BIT)\
    M(ShaderStage, FRAGMENT, VK_SHADER_STAGE_FRAGMENT_BIT)

#define VK_MSAA_COUNT_MAPPING_LIST(M)\
    M(MSAASamples, COUNT_1,  VK_SAMPLE_COUNT_1_BIT)\
    M(MSAASamples, COUNT_2,  VK_SAMPLE_COUNT_2_BIT)\
    M(MSAASamples, COUNT_4,  VK_SAMPLE_COUNT_4_BIT)\
    M(MSAASamples, COUNT_8,  VK_SAMPLE_COUNT_8_BIT)\
    M(MSAASamples, COUNT_16, VK_SAMPLE_COUNT_16_BIT)\
    M(MSAASamples, COUNT_32, VK_SAMPLE_COUNT_32_BIT)\
    M(MSAASamples, COUNT_64, VK_SAMPLE_COUNT_64_BIT)

#define VK_BLEND_OP_MAPPING_LIST(M)\
    M(BlendOp, ADD,              VK_BLEND_OP_ADD)\
    M(BlendOp, SUBTRACT,         VK_BLEND_OP_SUBTRACT)\
    M(BlendOp, REVERSE_SUBTRACT, VK_BLEND_OP_REVERSE_SUBTRACT)\
    M(BlendOp, MIN,              VK_BLEND_OP_MIN)\
    M(BlendOp, MAX,              VK_BLEND_OP_MAX)\
    M(BlendOp, MULTIPLY,         VK_BLEND_OP_MULTIPLY_EXT)\
    M(BlendOp, SCREEN,           VK_BLEND_OP_SCREEN_EXT)\
    M(BlendOp, OVERLAY,          VK_BLEND_OP_OVERLAY_EXT)

#define VK_BLEND_FACTOR_MAPPING_LIST(M)\
    M(BlendFactor, ZERO,                     VK_BLEND_FACTOR_ZERO)\
    M(BlendFactor, ONE,                      VK_BLEND_FACTOR_ONE)\
    M(BlendFactor, SRC_COLOR,                VK_BLEND_FACTOR_SRC_COLOR)\
    M(BlendFactor, ONE_MINUS_SRC_COLOR,      VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR)\
    M(BlendFactor, DST_COLOR,                VK_BLEND_FACTOR_DST_COLOR)\
    M(BlendFactor, ONE_MINUS_DST_COLOR,      VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR)\
    M(BlendFactor, SRC_ALPHA,                VK_BLEND_FACTOR_SRC_ALPHA)\
    M(BlendFactor, ONE_MINUS_SRC_ALPHA,      VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA)\
    M(BlendFactor, DST_ALPHA,                VK_BLEND_FACTOR_DST_ALPHA)\
    M(BlendFactor, ONE_MINUS_DST_ALPHA,      VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA)\
    M(BlendFactor, CONSTANT_COLOR,           VK_BLEND_FACTOR_CONSTANT_COLOR)\
    M(BlendFactor, ONE_MINUS_CONSTANT_COLOR, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR)\
    M(BlendFactor, CONSTANT_ALPHA,           VK_BLEND_FACTOR_CONSTANT_ALPHA)\
    M(BlendFactor, ONE_MINUS_CONSTANT_ALPHA, VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA)\
    M(BlendFactor, SRC_ALPHA_SATURATE,       VK_BLEND_FACTOR_SRC_ALPHA_SATURATE)\
    M(BlendFactor, SRC1_COLOR,               VK_BLEND_FACTOR_SRC1_COLOR)\
    M(BlendFactor, ONE_MINUS_SRC1_COLOR,     VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR)\
    M(BlendFactor, SRC1_ALPHA,               VK_BLEND_FACTOR_SRC1_ALPHA)\
    M(BlendFactor, ONE_MINUS_SRC1_ALPHA,     VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA)

#define VK_SHADER_RESOURCE_TYPE_MAPPING_LIST(M)\
    M(ShaderResourceType, SAMPLER,                VK_DESCRIPTOR_TYPE_SAMPLER)\
    M(ShaderResourceType, COMBINED_IMAGE_SAMPLER, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)\
    M(ShaderResourceType, SAMPLED_IMAGE,          VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE)\
    M(ShaderResourceType, STORAGE_IMAGE,          VK_DESCRIPTOR_TYPE_STORAGE_IMAGE)\
    M(ShaderResourceType, UNIFORM_TEXEL_BUFFER,   VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER)\
    M(ShaderResourceType, STORAGE_TEXEL_BUFFER,   VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER)\
    M(ShaderResourceType, UNIFORM_BUFFER,         VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)\
    M(ShaderResourceType, STORAGE_BUFFER,         VK_DESCRIPTOR_TYPE_STORAGE_BUFFER)\
    M(ShaderResourceType, UNIFORM_BUFFER_DYNAMIC, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)\
    M(ShaderResourceType, STORAGE_BUFFER_DYNAMIC, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC)\
    M(ShaderResourceType, INPUT_ATTACHMENT,       VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT)

#define CROSS_EXPAND_VK_CASE(enumType, name, vk) case Vox::Render::enumType::name: return vk;
#define CROSS_EXPAND_VK_CASE_FROM(enumType, name, vk) case vk: return Vox::Render::enumType::name;

#define DEFINE_ENUM_CONVERTER_VK(EnumType, ApiEnumType, MAPPING_LIST)\
constexpr inline ApiEnumType toVk(Vox::Render::EnumType format) {\
    switch (format) {\
        MAPPING_LIST(CROSS_EXPAND_VK_CASE)\
        default:\
            LOG_ERROR("Unmapped enum value: {}", (int)format);\
            VOX_ASSERT(false, "Enum value not mapped to Vulkan");\
    };\
}

#define DEFINE_ENUM_CONVERTER_FROM_VK(EnumType, ApiEnumType, MAPPING_LIST)\
constexpr inline Vox::Render::EnumType fromVk(ApiEnumType format) {\
    switch (format) {\
        MAPPING_LIST(CROSS_EXPAND_VK_CASE_FROM)\
        default:\
            LOG_ERROR("Unmapped Vulkan value: {}", (int)format);\
            VOX_ASSERT(false, "Vulkan value not mapped to Enum");\
    }\
    __builtin_unreachable();\
}

    DEFINE_ENUM_CONVERTER_VK(Format, VkFormat, VK_FORMAT_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(Format, VkFormat, VK_FORMAT_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(ShaderStage, VkShaderStageFlagBits, VK_SHADER_STAGE_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(ShaderStage, VkShaderStageFlagBits, VK_SHADER_STAGE_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(IndexType, VkIndexType, VK_INDEX_TYPE_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(IndexType, VkIndexType, VK_INDEX_TYPE_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(BufferUsage, VkBufferUsageFlagBits, VK_BUFFER_USAGE_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(BufferUsage, VkBufferUsageFlagBits, VK_BUFFER_USAGE_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(PrimitiveTopology, VkPrimitiveTopology, VK_PRIMITIVE_TOPOLOGY_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(PrimitiveTopology, VkPrimitiveTopology, VK_PRIMITIVE_TOPOLOGY_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(PolygonMode, VkPolygonMode, VK_POLYGON_MODE_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(PolygonMode, VkPolygonMode, VK_POLYGON_MODE_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(CullMode, VkCullModeFlags, VK_CULL_MODE_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(CullMode, VkCullModeFlags, VK_CULL_MODE_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(FrontFace, VkFrontFace, VK_FRONT_FACE_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(FrontFace, VkFrontFace, VK_FRONT_FACE_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(BlendFactor, VkBlendFactor, VK_BLEND_FACTOR_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(BlendFactor, VkBlendFactor, VK_BLEND_FACTOR_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(BlendOp, VkBlendOp, VK_BLEND_OP_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(BlendOp, VkBlendOp, VK_BLEND_OP_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(MSAASamples, VkSampleCountFlagBits, VK_MSAA_COUNT_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(MSAASamples, VkSampleCountFlagBits, VK_MSAA_COUNT_MAPPING_LIST);

    DEFINE_ENUM_CONVERTER_VK(ShaderResourceType, VkDescriptorType, VK_SHADER_RESOURCE_TYPE_MAPPING_LIST);
    DEFINE_ENUM_CONVERTER_FROM_VK(ShaderResourceType, VkDescriptorType, VK_SHADER_RESOURCE_TYPE_MAPPING_LIST);

    constexpr inline uint8_t IndexTypeSize(IndexType type) {
        switch (type) {
            case IndexType::UINT8: return 1;
            case IndexType::UINT16: return 2;
            case IndexType::UINT32: return 4;
        }
    }


    constexpr inline VkExtent2D toVk(const Extent &extent) { return {extent.width, extent.height}; }

    constexpr inline VkExtent2D toVk(const Extent &&extent) { return {extent.width, extent.height}; }

    constexpr inline VkExtent3D toVk(const Extent3D &extent) { return {extent.width, extent.height, extent.depth}; }

    constexpr inline VkExtent3D toVk(const Extent3D &&extent) { return {extent.width, extent.height, extent.depth}; }

    constexpr inline Extent fromVk(const VkExtent2D &extent) { return {extent.width, extent.height}; }

    constexpr inline Extent fromVk(const VkExtent2D &&extent) { return {extent.width, extent.height}; }

NS_END
