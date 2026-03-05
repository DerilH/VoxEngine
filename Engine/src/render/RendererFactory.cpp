//
// Created by deril on 2/17/26.
//

#include <VoxEngine/render/RendererFactory.h>

RENDER_NS
    Renderer* RendererFactory::Create(const RenderAPI api) {
        return new Renderer(api);
    }
NS_END
