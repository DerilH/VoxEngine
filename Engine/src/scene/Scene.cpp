#include "VoxEngine/scene/Scene.h"

#include <VoxEngine/scene/GameObject.h>

Vox::Scene::Scene::Scene(InternedString name, Ref<GameObjectRegistry> objectRegistry) : mName(name), mObjectRegistry(objectRegistry) {
}

Vox::Scene::GameObjectRef Vox::Scene::Scene::createObject(InternedString name) {
    const auto object = new GameObject(name, mObjectRegistry->allocGameObjectId());
    mObjectRegistry->add(object->id, object);
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

void Vox::Scene::Scene::addRenderable(Ref<RendererComponent> renderable) {
    mRenderables.emplace(renderable);
}

void Vox::Scene::Scene::removeRenderable(Ref<RendererComponent> renderable) {
    mRenderables.erase(renderable);
}

void Vox::Scene::Scene::addCamera(Ref<CameraComponent> camera) {
    mCameras.emplace(camera);
}

void Vox::Scene::Scene::removeCamera(Ref<CameraComponent> camera) {
    mCameras.erase(camera);
}