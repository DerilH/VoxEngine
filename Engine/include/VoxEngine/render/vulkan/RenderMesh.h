//
// Created by deril on 2/18/26.
//

#pragma once
#include <VoxCore/Define.h>
#include "VoxEngine/render/vulkan/buffers/VertexBuffer.h"
#include "VoxEngine/render/vulkan/buffers/IndexBuffer.h"
#include "CommandBuffer.h"

VULKAN_NS
class RenderMesh {
    const VertexBuffer* mVBuffer;
    const IndexBuffer* mIBuffer;

    NO_COPY_MOVE_DEFAULT(RenderMesh)

public:
    RenderMesh(const VertexBuffer *mVBuffer, const IndexBuffer *mIBuffer);

    void use(const CommandBuffer& cmdBuffer) const;
    int getIndexCount() const;
};
NS_END
