//
// Created by deril on 3/1/26.
//

#pragma once

#include "VoxCore/Define.h"
RENDER_NS
    typedef enum {
        VULKAN_API,
        OPENGL_API,
        DX11_API,
        DX12_API
    } RenderAPI;

class RenderBackend;
RenderBackend* CreateRenderBackend(RenderAPI api);
NS_END
