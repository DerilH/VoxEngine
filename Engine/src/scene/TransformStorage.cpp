#include "VoxEngine/scene/TransformStorage.h"

#include <VoxEngine/scene/components/Transform.h>
SCENE_NS
    Ref<Transform> TransformStorage::findObject(InternedString name) const {
        for (const auto el: mObjects) {
            if (el->gameObject->getName() == name) return el;
        }
        return nullptr;
    }

    void TransformStorage::addObject(Ref<Transform> object) {
        mObjects.emplace_back(object);
    }

    void TransformStorage::removeObject(Ref<Transform> object) {
        auto it = std::ranges::find(mObjects, object);
        if (it != mObjects.end()) {
            mObjects.erase(it);
        }
    }

    const Vector<Ref<Transform> > &TransformStorage::getAll() const {
        return mObjects;
    }

NS_END
