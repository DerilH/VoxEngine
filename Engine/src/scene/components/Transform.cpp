#include "VoxEngine/scene/components/Transform.h"

#include <VoxEngine/scene/TransformStorage.h>

Vox::Scene::Transform::Transform(Ref<GameObject> gameObject) : Component(gameObject), mChilds(new TransformStorage()) {
}
