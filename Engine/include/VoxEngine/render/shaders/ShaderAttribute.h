//
// Created by deril on 3/15/26.
//

#pragma once

#include <utility>

#include "VoxCore/Define.h"
#include "VoxCore/containers/Containers.h"
#include "ShaderDataType.h"

SHADERS_NS

    struct ShaderAttribute {
        enum Direction {
            IN, OUT
        } direction;
        InternedString name;
        uint32_t location;
        ShaderDataType typeId;
        uint32_t components;

        ShaderAttribute(InternedString name, ShaderDataType type, uint32_t location, uint32_t components, Direction direction) : typeId(type), name(std::move(name)), location(location), components(components), direction(direction) {}
    };
NS_END
