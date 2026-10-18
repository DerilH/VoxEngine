#include "VoxEngine/scene/components/MeshRendererComponent.h"
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/render/Renderer.h>
#include <VoxEngine/scene/Scene.h>

Vox::Scene::MeshRendererComponent::MeshRendererComponent(const GameObjectRef gameObject, const RenderLayer layer)
    : RendererComponent(gameObject, RenderableType::MESH), mLayer(layer) {
}

void Vox::Scene::MeshRendererComponent::OnAddedToScene(const Ref<Scene> scene) {
    scene->addRenderable(this);
}
