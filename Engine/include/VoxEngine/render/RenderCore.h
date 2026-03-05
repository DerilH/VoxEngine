//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "RenderBackend.h"
#include "VoxEngine/render/vulkan/VulkanBackend.h"

RENDER_NS
    typedef enum {
        VULKAN_API,
        OPENGL_API,
        DX11_API,
        DX12_API
    } RenderAPI;

    void InitRenderBackend(RenderAPI api);
NS_END
