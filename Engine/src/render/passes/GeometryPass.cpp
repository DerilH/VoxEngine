//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/passes/GeometryPass.h"
#include "VoxEngine/render/Renderer.h"

RENDER_NS
    void GeometryPass::execute(RenderContext context) {

        for (auto &entry: context.renderer->getDrawLists()) {
            auto state = context.renderer->getPipelineState(entry.first);
            state->bind(context.cmdBuffer);
            for(const auto& item: entry.second) {
                item.execute(context.cmdBuffer);
            }
        }
    }

    GeometryPass::GeometryPass(RenderPassType mType, const ArrayView<AttachmentDesc>& reads, const ArrayView<AttachmentDesc>& writes) : RenderPass(mType, reads, writes) {

    }
NS_END


