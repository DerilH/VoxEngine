//
// Created by deril on 2/17/26.
//

#include <VoxEngine/render/RendererFactory.h>
#include <VoxEngine/render/Renderer.h>

RENDER_NS
    Renderer* RendererFactory::Create(RenderAPI api, Ref<Resources::AssetManager> resManager) {
        auto back = CreateRenderBackend(api);
        auto res = CreateRenderResourceManager(back);

        return new Renderer(resManager, res, back);
    }
NS_END
