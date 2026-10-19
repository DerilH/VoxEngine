//
// Created by deril on 3/14/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/RenderMesh.h"

RENDER_NS
struct DrawItem {
    RenderMesh* mesh;
    glm::mat4 modelMat;
    uint64_t instanceId;

    DrawItem(RenderMesh* mesh, glm::mat4 modelMat, uint64_t instanceId) : mesh(mesh), modelMat(modelMat), instanceId(instanceId) {
    }

    void execute(CommandBufferRef cmdBuffer) const {
        mesh->use(cmdBuffer);
        cmdBuffer->drawIndexed(mesh->getIndexCount());
    }
};
NS_END
