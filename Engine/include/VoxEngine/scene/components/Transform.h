#pragma once
#include <glm/vec3.hpp>
#include <VoxCore/Define.h>
#include <VoxCore/math/Types.h>

#include "Component.h"

SCENE_NS
    class GameObjectStorage;
    class GameObject;

    class Transform : Component {
        friend GameObject;

        Vec3 mPosition = Vec3(0);
        Vec3 mScale = Vec3(1);
        Quat mRotation = Quat(1,0,0,0);

        Vec3 mLocalPosition = Vec3(0);
        Vec3 mLocalScale = Vec3(1);
        Quat mLocalRotation = Quat(1,0,0,0);

        Ref<GameObject> parent = nullptr;
        Ref<GameObjectStorage> childs;

        explicit Transform(Ref<GameObject> gameObject);
    public:

        Vec3 getPos() const {
            return mPosition;
        }

        Vec3 getScale() const {
            return mScale;
        }

        Quat getRotation() const {
            return mRotation;
        }

        void setPos(const Vec3 pos) {
            mPosition = pos;
        }

        void setScale(const Vec3 scale) {
            mScale = scale;
        }

        void setRotation(const Quat rot) {
            mRotation = rot;
        }

        NO_COPY_MOVE_DEFAULT(Transform);

        ComponentType type() override {
            return StaticType();
        }

        static ComponentType StaticType() {
            return ComponentType::TRANSFORM;
        }
    };
NS_END;
