//
// Created by deril on 2/18/26.
//

#include "VoxEngine/render/RenderMesh.h"

RENDER_NS
    RenderMesh::RenderMesh(const VertexBufferRef mVBuffer, const IndexBufferRef mIBuffer) : mVBuffer(mVBuffer), mIBuffer(mIBuffer) {}

    void RenderMesh::use(const CommandBufferRef cmdBuffer) const {
        mVBuffer->bind(cmdBuffer);
        mIBuffer->bind(cmdBuffer);
        if (mModelUbo) {
            mModelUbo->bind(cmdBuffer, 0);
        }
    }

    uint32_t RenderMesh::getIndexCount() const{
        return mIBuffer->getCount();
}

NS_END

