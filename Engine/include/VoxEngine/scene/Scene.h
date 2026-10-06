#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>

#include "Types.h"
#include "components/RendererComponent.h"
SCENE_NS
    class Scene {
        Vector<GameObjectRef> mRootObjects;
        InternedString mName;
        HashSet<Ref<RendererComponent> > mRenderables;

    public:
        explicit Scene(InternedString name);

        GameObjectRef createObject(InternedString name);

        void removeObject(GameObjectRef gameObject);

        const Vector<GameObjectRef> &getRootObjects();

        void addRenderable(Ref<RendererComponent> renderable);

        void removeRenderable(Ref<RendererComponent> renderable);

        HashSet<Ref<RendererComponent> > getAllRenderables();

        NO_COPY_MOVE(Scene)
    };

NS_END
