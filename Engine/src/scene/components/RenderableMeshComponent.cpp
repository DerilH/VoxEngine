#include "VoxEngine/scene/components/RenderableMeshComponent.h"
#include <VoxEngine/scene/GameObject.h>
#include <VoxEngine/render/Renderer.h>
#include <VoxEngine/scene/Scene.h>

Vox::Scene::RenderableMeshComponent::RenderableMeshComponent(const GameObjectRef gameObject) : RenderableComponent(gameObject, RenderableType::MESH) {
}

void Vox::Scene::RenderableMeshComponent::setMeshProvider(std::function<Ref<Resources::MeshAsset>()> meshProvider) {
    mMeshProvider = meshProvider;
}

void Vox::Scene::RenderableMeshComponent::setVertexShader(Ref<Resources::ShaderAsset> vertex) {
    mVertexShader = vertex;
}

void Vox::Scene::RenderableMeshComponent::setFragmentShader(Ref<Resources::ShaderAsset> fragment) {
    mFragmentShader = fragment;
}

void Vox::Scene::RenderableMeshComponent::OnAddedToScene(const Ref<Scene> scene) {
    scene->addRenderable(this);
}

Vox::Ref<Vox::Resources::MeshAsset> Vox::Scene::RenderableMeshComponent::getMesh() {
    return mMeshProvider();
}

Vox::Ref<Vox::Resources::ShaderAsset> Vox::Scene::RenderableMeshComponent::getVertexShader() {
    return mVertexShader;
}

Vox::Ref<Vox::Resources::ShaderAsset> Vox::Scene::RenderableMeshComponent::getFragmentShader() {
    return mFragmentShader;
}
