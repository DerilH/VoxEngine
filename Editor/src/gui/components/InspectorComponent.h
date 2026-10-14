#pragma once
#include <VoxCore/math/Types.h>
#include <VoxEngine/Engine.h>
#include <VoxEngine/render/RenderContext.h>
#include <VoxEngine/scene/GameObject.h>
#include "GuiComponent.h"

namespace Vox::Editor {
    class InspectorComponent : public GuiComponent {
        Ref<Scene::GameObject> mGameObject = nullptr;
        Vec3 mMeshRotation{};
        Ref<Engine> mEngine;

        void renderComponents();
        void renderTransformOptions();

    protected:
        void renderInternal(Render::RenderContext context) override;

    public:
        explicit InspectorComponent(Ref<Engine> engine);

        void setObject(Ref<Scene::GameObject> gameObject);
        NO_COPY_MOVE(InspectorComponent)
    };
}
