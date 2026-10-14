#include <VoxEngine/scene/components/CameraComponent.h>
#include <VoxEngine/scene/Scene.h>

SCENE_NS
void CameraComponent::OnAddedToScene(const Ref<Scene> scene) {
    scene->addCamera(this);
}
NS_END
