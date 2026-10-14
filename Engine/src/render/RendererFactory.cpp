//
// Created by deril on 2/17/26.
//

#include <VoxEngine/render/RendererFactory.h>
#include <VoxEngine/resources/ResourceManager.h>

RENDER_NS
    Renderer* RendererFactory::Create(RenderBackend *backend, Ref<Resources::ResourceManager> resManager) {
        return new Renderer(resManager, backend);
    }
NS_END
