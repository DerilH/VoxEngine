#include "VoxEngine/scene/Scene.h"

#include <VoxEngine/scene/GameObject.h>

Vox::Scene::Scene::Scene(InternedString name) : mName(name) {
}

Vox::Scene::GameObjectRef Vox::Scene::Scene::createObject(InternedString name) {
    const auto object = new GameObject(name);
    object->setScene(this);
    object->OnAddedToScene(this);
    mRootObjects.emplace_back(object);
    return object;
}

void Vox::Scene::Scene::removeObject(GameObjectRef gameObject) {
    auto it = std::ranges::find(mRootObjects, gameObject);
    if (it != mRootObjects.end()) {
        mRootObjects.erase(it);
    }
}

const Vox::Vector<Vox::Scene::GameObject *> &Vox::Scene::Scene::getRootObjects() {
    return mRootObjects;
}

void Vox::Scene::Scene::addRenderable(Ref<RenderableComponent> renderable) {
    mRenderables.emplace(renderable);
}

void Vox::Scene::Scene::removeRenderable(Ref<RenderableComponent> renderable) {
    mRenderables.erase(renderable);
}

Vox::HashSet<Vox::Scene::RenderableComponent *> Vox::Scene::Scene::getAllRenderables() {
    return mRenderables;
}
