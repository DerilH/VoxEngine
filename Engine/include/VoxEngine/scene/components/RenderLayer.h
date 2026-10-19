#pragma once
#include <VoxCore/Define.h>
#include <cstdint>

SCENE_NS
    enum class RenderLayer : uint8_t {
        WORLD = 1,
        UI = 2,
        GIZMO = 3
    };
NS_END
