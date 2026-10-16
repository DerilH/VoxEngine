//
// Created by deril on 10/13/26.
//

#pragma once

#include "GuiComponent.h"
#include <imgui.h>
#include <VoxEngine/Engine.h>
#include <VoxEngine/render/RenderContext.h>

namespace Vox::Editor {
    class ViewportComponent : public GuiComponent {
    bool mRegistered = false;
    ImTextureID mTextureId = 0;
    std::string buffer;
    Ref<Engine> mEngine;

    public:
        ViewportComponent(Ref<Engine> mEngine) : mEngine(mEngine) {
        }
    protected:
        void renderInternal(Render::RenderContext context) override;
    };
}
