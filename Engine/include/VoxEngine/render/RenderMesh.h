//
// Created by deril on 2/18/26.
//

#pragma once
#include "VoxCore/Define.h"
#include "VoxEngine/render/buffers/VertexBuffer.h"
#include "VoxEngine/render/buffers/IndexBuffer.h"
#include "VoxEngine/render/CommandBuffer.h"

RENDER_NS
class RenderMesh {
    const VertexBufferRef mVBuffer;
    const IndexBufferRef mIBuffer;

    NO_COPY_MOVE_DEFAULT(RenderMesh)
public:
    RenderMesh(const VertexBufferRef mVBuffer, const IndexBufferRef mIBuffer);

    void use(const CommandBufferRef cmdBuffer) const;
    uint32_t getIndexCount() const;
};
NS_END
