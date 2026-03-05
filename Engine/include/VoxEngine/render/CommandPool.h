//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "Types.h"

RENDER_NS
class CommandPool {
    virtual CommandBufferRef allocBuffer() = 0;
};
NS_END
