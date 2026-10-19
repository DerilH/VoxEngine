//
// Created by deril on 3/3/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "RenderBuffer.h"

RENDER_NS
    class TransferBuffer : virtual public RenderBuffer {
    protected:
        explicit TransferBuffer() {
        };

    public:
        virtual ArrayView<void> getSpan(size_t size) const = 0;
    };

NS_END
