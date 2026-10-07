//
// Created by deril on 2/27/26.
//

#pragma once
#include <imgui.h>
#include <VoxEngine/render/passes/RenderPass.h>
#include "Gui.h"

namespace Vox::Editor {
    class GuiRenderPass : public Render::RenderPass{
    public:
        Gui* mGui;
        GuiRenderPass(Render::RenderPassType mType, ArrayView<Render::AttachmentDesc> reads, ArrayView<Render::AttachmentDesc> writes, Gui* gui) : RenderPass(mType, reads, writes), mGui(gui) {}

        void execute(Render::RenderContext context) override {
            auto text = mReads[0].texture->getExact();
            mGui->render(context, text);
        }
        bool shouldClear() override {
            return true;
        }
    };
}
