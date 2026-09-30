//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/vulkan/VulkanCommandBuffer.h"
#include "VoxEngine/render/vulkan/buffers/VulkanVertexBuffer.h"
#include "VoxEngine/render/vulkan/buffers/VulkanIndexBuffer.h"
#include "VoxEngine/render/vulkan/VulkanTexture.h"
#include "VoxEngine/render/vulkan/buffers/VulkanTransferBuffer.h"
#include "VoxEngine/render/vulkan/state/VulkanPipelineState.h"

VULKAN_NS
    template<typename Type>
    struct VkResourceTraits {};

#define CAST_TRAIT(T, CastT) template<>  struct VkResourceTraits<T> { using CastType = CastT; };

    CAST_TRAIT(CommandBuffer, VulkanCommandBuffer)
    CAST_TRAIT(Texture, VulkanTexture)
    CAST_TRAIT(Device, VulkanDevice)
    CAST_TRAIT(IndexBuffer, VulkanIndexBuffer)
    CAST_TRAIT(VertexBuffer, VulkanVertexBuffer)
    CAST_TRAIT(TransferBuffer, VulkanTransferBuffer)
    CAST_TRAIT(RenderBuffer, VulkanRenderBuffer)
    CAST_TRAIT(PipelineState, VulkanPipelineState)

#undef CAST_TRAIT

    template<typename Type>
    constexpr static VkResourceTraits<Type>::CastType* ResourceCast(Type* Resource) {
        return static_cast<VkResourceTraits<Type>::CastType*>(Resource);
    }

    template<>
    constexpr VulkanRenderBuffer* ResourceCast(RenderBufferRef Resource) {
        return dynamic_cast<VulkanRenderBuffer*>(Resource);
    }
NS_END

