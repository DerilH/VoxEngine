//
// Created by deril on 2/28/26.
//

#pragma once

#include <vulkan/vulkan_core.h>
#include "VoxCore/math/Extent.h"
#include "VoxCore/math/Extent3D.h"
#include "VoxCore/Define.h"
#include "VoxEngine/render/Enums.h"
#include "VoxCore/Logger.h"

VULKAN_NS
    DEFINE_ENUM_CONVERTER_VK(Format, VkFormat, FORMAT_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(Format, VkFormat, FORMAT_LIST);

    DEFINE_ENUM_CONVERTER_VK(ShaderStage, VkShaderStageFlagBits, SHADER_STAGE_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(ShaderStage, VkShaderStageFlagBits, SHADER_STAGE_LIST);

    DEFINE_ENUM_CONVERTER_VK(IndexType, VkIndexType, INDEX_TYPE_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(IndexType, VkIndexType, INDEX_TYPE_LIST);

    DEFINE_ENUM_CONVERTER_VK(BufferUsage, VkBufferUsageFlagBits, BUFFER_USAGE_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(BufferUsage, VkBufferUsageFlagBits, BUFFER_USAGE_LIST);

    DEFINE_ENUM_CONVERTER_VK(PrimitiveTopology, VkPrimitiveTopology, PRIMITIVE_TOPOLOGY_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(PrimitiveTopology, VkPrimitiveTopology, PRIMITIVE_TOPOLOGY_LIST);

    DEFINE_ENUM_CONVERTER_VK(PolygonMode, VkPolygonMode, POLYGON_MODE_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(PolygonMode, VkPolygonMode, POLYGON_MODE_LIST);

    DEFINE_ENUM_CONVERTER_VK(CullMode, VkCullModeFlagBits, CULL_MODE_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(CullMode, VkCullModeFlagBits, CULL_MODE_LIST);

    DEFINE_ENUM_CONVERTER_VK(FrontFace, VkFrontFace, FRONT_FACE_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(FrontFace, VkFrontFace, FRONT_FACE_LIST);

    DEFINE_ENUM_CONVERTER_VK(BlendFactor, VkBlendFactor, BLEND_FACTOR_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(BlendFactor, VkBlendFactor, BLEND_FACTOR_LIST);

    DEFINE_ENUM_CONVERTER_VK(BlendOp, VkBlendOp, BLEND_OP_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(BlendOp, VkBlendOp, BLEND_OP_LIST);

    DEFINE_ENUM_CONVERTER_VK(MSAASamples, VkSampleCountFlagBits, MSAA_COUNT_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(MSAASamples, VkSampleCountFlagBits, MSAA_COUNT_LIST);

    DEFINE_ENUM_CONVERTER_VK(ShaderResourceType, VkDescriptorType, SHADER_RESOURCE_TYPE_LIST);

    DEFINE_ENUM_CONVERTER_FROM_VK(ShaderResourceType, VkDescriptorType, SHADER_RESOURCE_TYPE_LIST);


    constexpr inline uint8_t IndexTypeSize(IndexType type) {
        switch (type) {
            case IndexType::UINT8: return 1;
            case IndexType::UINT16: return 2;
            case IndexType::UINT32: return 4;
        }
    }


    constexpr inline VkExtent2D toVk(const Extent& extent) { return {extent.width, extent.height}; }

    constexpr inline VkExtent2D toVk(const Extent&& extent) { return {extent.width, extent.height}; }

    constexpr inline VkExtent3D toVk(const Extent3D& extent) { return {extent.width, extent.height, extent.depth}; }

    constexpr inline VkExtent3D toVk(const Extent3D&& extent) { return {extent.width, extent.height, extent.depth}; }

    constexpr inline Extent fromVk(const VkExtent2D& extent) { return {extent.width, extent.height}; }

    constexpr inline Extent fromVk(const VkExtent2D&& extent) { return {extent.width, extent.height}; }

NS_END
