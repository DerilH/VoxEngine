#include "VoxEngine/scene/components/Transform.h"

#include <VoxEngine/scene/GameObjectStorage.h>

Vox::Scene::Transform::Transform(Ref<GameObject> gameObject) : Component(gameObject), mChilds(new GameObjectStorage()) {
}
