#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
SCENE_NS
    class GameObject;
    class Scene;

    class Component {
    public:
        virtual ~Component() = default;


        explicit Component(const Ref<GameObject> gameObject) : gameObject(gameObject) {
        }

        virtual void OnAddedToScene(Ref<Scene> scene);

        const Ref<GameObject> gameObject;
    };

NS_END
