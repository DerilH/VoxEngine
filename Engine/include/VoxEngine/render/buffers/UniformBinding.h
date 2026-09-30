//
// Created by deril on 3/25/26.
//

#pragma once
#include <cstdint>
#include <VoxCore/Define.h>

RENDER_NS
class UniformBinding {
    const uint8_t set;
    const uint8_t binding;
public:
    UniformBinding(const uint8_t set, const uint8_t binding)
        : set(set),
          binding(binding) {
    }
};
NS_END