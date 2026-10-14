//
// Created by deril on 10/13/26.
//

#pragma once

#include "GuiComponent.h"
#include <imgui.h>
#include <VoxEngine/render/RenderContext.h>

namespace Vox::Editor {
    class ViewportComponent : public GuiComponent {
    bool mRegistered = false;
    ImTextureID mTextureId = 0;

    protected:
        void renderInternal(Render::RenderContext context) override;
    };
}
