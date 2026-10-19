#pragma once
#include "GuiComponent.h"
#include <VoxCore/Pointers.h>
#include <VoxEngine/Engine.h>
#include <VoxEngine/scene/Scene.h>

namespace Vox::Editor {
    class HierarchyComponent : public GuiComponent {
        Ref<Scene::GameObject> mSelected = nullptr;
        Ref<Scene::Scene> mScene = nullptr;
        Ref<Engine> mEngine;
        char mFilterBuffer[64] = "";

        void renderNode(Scene::GameObjectRef el);
        void duplicateObject(Scene::GameObjectRef orig);

    protected:
        void renderInternal(Render::RenderContext context) override;

    public:
        explicit HierarchyComponent(Ref<Engine> engine);
        Event<Ref<Scene::GameObject>> onSelectedChanged;

        Ref<Scene::GameObject> getSelected() const {
            return mSelected;
        }
        void setScene(Ref<Scene::Scene> scene) {
            mScene = scene;
        }

        void setSelected(Ref<Scene::GameObject> selected) {
            mSelected = selected;
        }
        NO_COPY_MOVE_DEFAULT(HierarchyComponent);
    };
}
