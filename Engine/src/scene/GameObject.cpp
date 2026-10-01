//
// Created by deril on 10/1/26.
//

#include "VoxEngine/scene/GameObject.h"
#include "VoxEngine/scene/components/Transform.h"
#include "VoxEngine/scene/Scene.h"

void Vox::Scene::GameObject::setScene(Ref<Scene> scene) {
    mScene = scene;
}

Vox::Scene::GameObject::GameObject(InternedString name) : transform(new Transform(this)) {
    mName = name;
    attachComponent(transform);
}

Vox::InternedString Vox::Scene::GameObject::getName() {
    return mName;
}

void Vox::Scene::GameObject::setName(InternedString name) {
    mName = name;
}

void Vox::Scene::GameObject::attachComponent(Ref<Component> component) {
    mComponents.emplace_back(component);
    component->OnAddedToScene(mScene);
}

void Vox::Scene::GameObject::detachComponent(Ref<Component> component) {
    auto res = std::find(mComponents.begin(), mComponents.end(), component);
    if (res != mComponents.end()) {
        mComponents.erase(res);
    }
}

void Vox::Scene::GameObject::OnAddedToScene(Ref<Scene> scene) {
    for (const auto el : mComponents) {
        el->OnAddedToScene(scene);
    }
}
