//
// Created by deril on 2/24/26.
//

#pragma once


#include "VoxCore/Define.h"

VULKAN_NS
class Barrier {
public:
    virtual void bind() = 0;
};
NS_END