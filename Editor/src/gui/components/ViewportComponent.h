//
// Created by deril on 10/13/26.
//

#pragma once

#include "GuiComponent.h"
#include <imgui.h>
#include <glm/vec3.hpp>
#include <VoxEngine/Engine.h>
#include <VoxEngine/render/RenderContext.h>
#include <VoxEngine/scene/components/CameraComponent.h>

#include "../GizmoPass.h"

namespace Vox::Editor {
    enum class GizmoElement : uint64_t {
        None    = 0,
        Arrow_X = -1ULL,
        Arrow_Y = -1ULL - 1,
        Arrow_Z = -1ULL - 2,
        Ring_X  = -1ULL - 3,
        Ring_Y  = -1ULL - 4,
        Ring_Z  = -1ULL - 5,
        Cube_X  = -1ULL - 6,
        Cube_Y  = -1ULL - 7,
        Cube_Z  = -1ULL - 8
    };

    inline bool isGizmoElement(uint64_t index) {
        return index >= static_cast<uint64_t>(GizmoElement::Cube_Z) &&
               index <= static_cast<uint64_t>(GizmoElement::Arrow_X);
    }

    inline bool isTranslateGizmoElement(GizmoElement el) {
        return el == GizmoElement::Arrow_X || el == GizmoElement::Arrow_Y || el == GizmoElement::Arrow_Z;
    }

    inline bool isRotateGizmoElement(GizmoElement el) {
        return el == GizmoElement::Ring_X || el == GizmoElement::Ring_Y || el == GizmoElement::Ring_Z;
    }

    inline bool isScaleGizmoElement(GizmoElement el) {
        return el == GizmoElement::Cube_X || el == GizmoElement::Cube_Y || el == GizmoElement::Cube_Z;
    }

    class ViewportComponent : public GuiComponent {
        bool mRegistered = false;
        ImTextureID mTextureId = 0;
        std::string buffer;
        Ref<Engine> mEngine;

        bool mIsCaptured = false;
        bool mIsPanning = false;
        bool mJustWarped = false;
        float mYaw = 0.0f;
        float mPitch = 0.0f;
        bool mAnglesInitialized = false;
        Ref<Scene::CameraComponent> mLastCamera = nullptr;
        Ref<GizmoPass> mGizmoPass;
        Ref<Scene::GameObject> mSelectedObj = nullptr;

        GizmoElement mActiveGizmoElement = GizmoElement::None;
        bool mIsDraggingGizmo = false;
        bool mJustStartedDrag = false;
        ImVec2 mClickMousePos = ImVec2(0.0f, 0.0f);
        glm::vec3 mArrowDirs[3] = {
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f),
            glm::vec3(0.0f, 0.0f, 1.0f)
        };
    public:
        Event<Ref<Scene::GameObject>> onObjectPicked;

        explicit ViewportComponent(Ref<Engine> mEngine, Ref<GizmoPass> gizmoPass);

        void setSelected(Ref<Scene::GameObject> obj) {
            mSelectedObj = obj;
            if (!mSelectedObj) {
                mActiveGizmoElement = GizmoElement::None;
                mIsDraggingGizmo = false;
                mJustStartedDrag = false;
            }
        }

        GizmoElement getActiveGizmoElement() const {
            return mActiveGizmoElement;
        }

        bool isDraggingGizmo() const {
            return mIsDraggingGizmo;
        }

        bool isPanning() const {
            return mIsPanning;
        }
    protected:
        void renderInternal(Render::RenderContext context) override;

    };
}
