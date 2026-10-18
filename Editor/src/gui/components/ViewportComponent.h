//
// Created by deril on 10/13/26.
//

#pragma once

#include "GuiComponent.h"
#include <imgui.h>
#include <VoxEngine/Engine.h>
#include <VoxEngine/render/RenderContext.h>
#include <VoxEngine/scene/components/CameraComponent.h>

namespace Vox::Editor {
    class ViewportComponent : public GuiComponent {
        bool mRegistered = false;
        ImTextureID mTextureId = 0;
        std::string buffer;
        Ref<Engine> mEngine;

        bool mIsCaptured = false;
        bool mJustWarped = false;
        float mYaw = 0.0f;
        float mPitch = 0.0f;
        bool mAnglesInitialized = false;
        Ref<Scene::CameraComponent> mLastCamera = nullptr;

    public:
        ViewportComponent(Ref<Engine> mEngine) : mEngine(mEngine) {
        }
    protected:
        void renderInternal(Render::RenderContext context) override;
    };
}
