#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>

#include "ComponentType.h"
SCENE_NS
    class GameObject;
    class Scene;

    class Component {
    public:
        virtual ~Component() = default;


        explicit Component(const Ref<GameObject> gameObject) : gameObject(gameObject) {
        }

        virtual void OnAddedToScene(Ref<Scene> scene);


        virtual ComponentType type() = 0;

        const Ref<GameObject> gameObject;
    };

NS_END
