//
// Created by deril on 2/28/26.
//

#pragma once
#include "VoxCore/Define.h"

RENDER_NS
class Renderer;
struct RenderContext {
    Renderer* renderer;
    const CommandBufferRef cmdBuffer;
    RenderContext(Renderer* renderer, CommandBufferRef cmdBuffer) : renderer(renderer), cmdBuffer(cmdBuffer) {}
};
NS_END
