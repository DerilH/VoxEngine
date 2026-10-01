#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>

#include "Types.h"
#include "components/RenderableComponent.h"
SCENE_NS
    class Scene {
        Vector<GameObjectRef> mRootObjects;
        InternedString mName;
        HashSet<Ref<RenderableComponent> > mRenderables;

    public:
        explicit Scene(InternedString name);

        GameObjectRef createObject(InternedString name);

        void removeObject(GameObjectRef gameObject);

        const Vector<GameObjectRef> &getRootObjects();

        void addRenderable(Ref<RenderableComponent> renderable);

        void removeRenderable(Ref<RenderableComponent> renderable);

        HashSet<Ref<RenderableComponent> > getAllRenderables();

        NO_COPY_MOVE(Scene)
    };

NS_END
