//
// Created by deril on 3/1/26.
//

#pragma once

#include <VoxCore/Pointers.h>

#include "VoxCore/Define.h"

RENDER_NS
    class RenderResourceManager;
    class RenderBackend;

    typedef enum {
        VULKAN_API,
        OPENGL_API,
        DX11_API,
        DX12_API
    } RenderAPI;


    RenderBackend *CreateRenderBackend(RenderAPI api);

    Ref<RenderResourceManager> CreateRenderResourceManager(Ref<RenderBackend> api);

NS_END
