//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "RenderBuffer.h"

RENDER_NS
    class TransferBuffer : virtual public RenderBuffer {
    protected:
        explicit TransferBuffer() {};
    };
NS_END
