//
// Created by deril on 2/17/26.
//

#pragma once
#include <VoxCore/Pointers.h>
#include <../../../../Editor/src/assets/ResourceManager.h>

#include "RenderCore.h"

RENDER_NS
    class Renderer;

    class RendererFactory {
        virtual void t() = 0;

        virtual ~RendererFactory() = default;

    public:
        static Renderer *Create(RenderAPI backend, Ref<Resources::AssetManager> resManager);
    };

NS_END
