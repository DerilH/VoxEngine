#pragma once
#include <VoxCore/Define.h>

#include "Component.h"

SCENE_NS
    class TransformStorage;
    class GameObject;

    class CameraComponent : public Component {
    public:
        explicit CameraComponent(Ref<GameObject> gameObject) : Component(gameObject) {}
        NO_COPY_MOVE_DEFAULT(CameraComponent);

        void OnAddedToScene(Ref<Scene> scene) override;

        ComponentType type() override {
            return StaticType();
        }

        static ComponentType StaticType() {
            return ComponentType::CAMERA;
        }
    };
NS_END;
