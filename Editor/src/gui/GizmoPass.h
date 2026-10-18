//
// Created by deril on 2/27/26.
//

#pragma once
#include <VoxEngine/render/passes/RenderPass.h>
#include "Gui.h"

namespace Vox::Editor {
    class GuiRenderPass : public Render::RenderPass{
    public:
        GuiRenderPass(Render::RenderPassType mType, ArrayView<Render::AttachmentDesc> reads, ArrayView<Render::AttachmentDesc> writes) : RenderPass(mType, reads, writes) {}

        void execute(Render::RenderContext context) override {
        }
        bool shouldClear() override {
            return true;
        }
    };
}
