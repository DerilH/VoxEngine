#pragma once
#include <VoxCore/Define.h>
#include <VoxEngine/render/RenderMesh.h>
#include <VoxEngine/scene/Types.h>

#include "Component.h"
SCENE_NS
    enum class RenderableType {
        MESH
    };

    class RenderableComponent : public Component {
    public:
        const RenderableType type;
        explicit RenderableComponent(const GameObjectRef gameObject, const RenderableType type) : Component(gameObject), type(type) {
        }
    };

NS_END
