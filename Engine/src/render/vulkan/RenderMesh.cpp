//
// Created by deril on 2/18/26.
//

#include "VoxEngine/render/vulkan/RenderMesh.h"

VULKAN_NS
    RenderMesh::RenderMesh(const VertexBuffer *mVBuffer, const IndexBuffer *mIBuffer) : mVBuffer(mVBuffer), mIBuffer(mIBuffer) {}

    void RenderMesh::use(const CommandBuffer& cmdBuffer) const {
        mVBuffer->bind(cmdBuffer);
        mIBuffer->bind(cmdBuffer);
    }

    int RenderMesh::getIndexCount() const{
        return mIBuffer->getCount();
}

NS_END

