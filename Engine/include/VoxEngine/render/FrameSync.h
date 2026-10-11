#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>

#include "CommandBuffer.h"
RENDER_NS
    class FrameSync {
    public:
        virtual ~FrameSync() = default;

    private:
        virtual Ref<CommandBuffer> getCmdBuffer() const = 0;
        virtual uint32_t begin() = 0;
        virtual void submit() = 0;
    };
NS_END
