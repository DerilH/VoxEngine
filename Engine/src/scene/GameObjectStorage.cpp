#include "VoxEngine/scene/GameObjectStorage.h"

Vox::Scene::GameObjectRef Vox::Scene::GameObjectStorage::findObject(InternedString name) const {
    for (const auto el : mObjects) {
        if (el->getName() == name) return el;
    }
    return nullptr;
}

void Vox::Scene::GameObjectStorage::addObject(GameObjectRef object) {
    mObjects.emplace_back(object);
}

void Vox::Scene::GameObjectStorage::removeObject(GameObjectRef object) {
    auto it = std::ranges::find(mObjects, object);
    if (it != mObjects.end()) {
        mObjects.erase(it);
    }
}

const Vox::Vector<Vox::Scene::GameObjectRef> & Vox::Scene::GameObjectStorage::getAll() {
    return mObjects;
}
