//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "Renderer.h"
#include "VoxEngine/render/vulkan/VulkanBackend.h"
#include "VoxEngine/render/Backend.h"

RENDER_NS
    typedef enum {
        VULKAN_API,
        OPENGL_API,
        DX11_API,
        DX12_API
    } RenderAPI;

    void InitRenderBackend(RenderAPI api) {
        switch (api) {
            case VULKAN_API:
                Backend::Init(Vulkan::VulkanBackend::Create());
            default:
                VOX_NO_IMPL("Unsupported render api");
        }
    }
NS_END