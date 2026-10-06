#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>
#include "components/Component.h"

SCENE_NS
    class Transform;
    class Scene;

    class GameObject {
        friend Scene;

    protected:
        InternedString mName;
        Vector<Ref<Component> > mComponents;
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


        //Returns pointer to found component or nullptr if not exists
        template<typename T>
            requires std::derived_from<T, Component>
        Ref<T> findComponent() {
            auto it = std::ranges::find_if(mComponents, [](const Ref<Component>& comp) {
                return comp && comp->type() == T::StaticType();
            });

            if (it == mComponents.end()) {
                return nullptr;
            }

            return static_cast<Ref<T>>(*it);
        }

        const Vector<Ref<Component> > &getComponent() {
            return mComponents;
        }
    };

NS_END
