//
// Created by deril on 2/17/26.
//

#include <VoxEngine/render/RendererFactory.h>

RENDER_NS
    Renderer* RendererFactory::Create(RenderBackend *backend) {
        return new Renderer(backend);
    }
NS_END
