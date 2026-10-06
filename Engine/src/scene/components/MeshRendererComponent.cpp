#include "VoxEngine/scene/components/MeshRendererComponent.h"
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/render/Renderer.h>
#include <VoxEngine/scene/Scene.h>

Vox::Scene::MeshRendererComponent::MeshRendererComponent(const GameObjectRef gameObject) : RendererComponent(gameObject, RenderableType::MESH) {
}

void Vox::Scene::MeshRendererComponent::OnAddedToScene(const Ref<Scene> scene) {
    scene->addRenderable(this);
}
