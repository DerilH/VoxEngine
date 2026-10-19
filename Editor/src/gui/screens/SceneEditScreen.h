#pragma once
#include <VoxCore/Pointers.h>
#include <VoxEngine/scene/GameObject.h>

#include "Screen.h"
#include "../AssetExplorer.h"

namespace Vox::Editor {
    class GizmoPass;

    class SceneEditScreen : public Screen {
        Ref<Scene::GameObject> mSelectedObject = nullptr;
    protected:
        void renderInternal(Render::RenderContext context) override;

    public:
        Event<Ref<Scene::GameObject>> onSelectedChanged;
        explicit SceneEditScreen(Ref<Engine> engine, Ref<GizmoPass> gizmoPass);
        NO_COPY_MOVE_DEFAULT(SceneEditScreen);
    };
}
