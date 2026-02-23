//
// Created by deril on 2/17/26.
//

#pragma once
#include "Renderer.h"

RENDER_NS
    class RendererFactory {
        virtual void t() = 0;

        virtual ~RendererFactory() = default;

    public:
        static Renderer *Create(Shader::ShaderRepository& shaderRepository, RenderAPI api);
    };

NS_END
