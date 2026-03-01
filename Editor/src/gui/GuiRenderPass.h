//
// Created by deril on 2/27/26.
//

#pragma once
#include <VoxEngine/render/vulkan/passes/RenderPass.h>
#include "Gui.h"

namespace Vox::Editor {
    class GuiRenderPass : public Render::Vulkan::RenderPass{
    public:
        Gui* mGui;
        GuiRenderPass(Render::RenderPassType mType, Vector<Render::AttachmentDesc> reads, Vector<Render::AttachmentDesc> writes, Gui* gui) : RenderPass(mType, reads, writes), mGui(gui) {}

        void execute(const Render::RenderContext& context) override {
            mGui->render(const_cast<Render::Vulkan::CommandBuffer&>(*mCmdBuffer));
        }
    };
}