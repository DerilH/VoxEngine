#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/containers/Containers.h>
#include "GameObject.h"
#include "VoxEngine/scene/Types.h"

SCENE_NS
    class GameObjectStorage {
        friend Transform;

        Vector<GameObjectRef> mObjects;

        GameObjectStorage() = default;

    public:
        GameObjectRef findObject(InternedString name) const;

        void addObject(GameObjectRef object);

        void removeObject(GameObjectRef object);

        const Vector<GameObjectRef> &getAll();

        NO_COPY_MOVE(GameObjectStorage)
    };

NS_END
