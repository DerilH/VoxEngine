//
// Created by deril on 3/14/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/RenderMesh.h"

RENDER_NS
struct DrawItem {
    RenderMesh* mesh;

    DrawItem(RenderMesh* mesh) : mesh(mesh) {
    }

    void execute(CommandBufferRef cmdBuffer) const {
        mesh->use(cmdBuffer);
        cmdBuffer->drawIndexed(mesh->getIndexCount());
    }
};
NS_END
