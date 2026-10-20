#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/containers/Containers.h>
#include "GameObject.h"
#include "VoxEngine/scene/Types.h"

SCENE_NS
    class TransformStorage {
        friend Transform;

        Vector<Ref<Transform>> mObjects;

        TransformStorage() = default;

    public:
        Ref<Transform> findObject(InternedString name) const;

        void addObject(Ref<Transform> object);

        void removeObject(Ref<Transform> object);

        const Vector<Ref<Transform>> &getAll() const;

        uint32_t count() const {
            return mObjects.size();
        }

        NO_COPY_MOVE(TransformStorage)
    };

NS_END
