#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>

SCENE_NS
    class Component;
    class Transform;
    class Scene;

    class GameObject {
        friend Scene;

    protected:
        InternedString mName;
        Vector<Ref<Component>> mComponents;
        Ref<Scene> mScene = nullptr;

    public:
        const Ref<Transform> transform;

        explicit GameObject(InternedString name);

        InternedString getName();

        void setName(InternedString name);

        void attachComponent(Ref<Component> component);

        void detachComponent(Ref<Component> component);

        void OnAddedToScene(Ref<Scene> scene);
        void setScene(Ref<Scene> scene);
    };

NS_END
