//
// Created by deril on 3/3/26.
//

#pragma once


#include <vulkan/vulkan_core.h>
#include "VoxCore/Define.h"
#include "VoxEngine/render/CommandBuffer.h"
#include "VoxEngine/render/Texture.h"

VULKAN_NS
    template<typename Type>
    struct VkResourceTraits {};

#define CAST_TRAIT(T, CastT) class CastT; template<>  struct VkResourceTraits<T> { using CastType = CastT; };

    CAST_TRAIT(CommandBuffer, VulkanCommandBuffer)
    CAST_TRAIT(Texture, VukanTexture)
    CAST_TRAIT(Device, VulkanDevice)

#undef CAST_TRAIT

    template<typename Type>
    static inline typename VkResourceTraits<Type>::CastType* ResourceCast(Type* Resource) {
        return static_cast<typename VkResourceTraits<Type>::CastType*>(Resource);
    }
NS_END
