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

        Vec3 mPosition;
        Vec3 mScale;
        Quat mRotation;

        Vec3 mLocalPosition;
        Vec3 mLocalScale;
        Quat mLocalRotation;

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
    };
NS_END;
