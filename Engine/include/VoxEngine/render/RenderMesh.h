//
// Created by deril on 2/18/26.
//

#pragma once
#include "VoxCore/Define.h"
#include "VoxEngine/render/buffers/VertexBuffer.h"
#include "VoxEngine/render/buffers/IndexBuffer.h"
#include "VoxEngine/render/CommandBuffer.h"
#include "VoxEngine/render/buffers/UniformBuffer.h"
#include "VoxEngine/render/vulkan/VulkanTypes.h"

RENDER_NS
class RenderMesh {
    const VertexBufferRef mVBuffer;
    const IndexBufferRef mIBuffer;
    UniformBufferRef mModelUbo = nullptr;
    Vulkan::VulkanDescriptorSetRef mDescriptorSet = nullptr;

    NO_COPY_MOVE_DEFAULT(RenderMesh)
public:
    RenderMesh(const VertexBufferRef mVBuffer, const IndexBufferRef mIBuffer);

    void use(const CommandBufferRef cmdBuffer) const;
    uint32_t getIndexCount() const;

    void setModelUbo(UniformBufferRef ubo) { mModelUbo = ubo; }
    UniformBufferRef getModelUbo() const { return mModelUbo; }
    void setDescriptorSet(Vulkan::VulkanDescriptorSetRef set) { mDescriptorSet = set; }
    Vulkan::VulkanDescriptorSetRef getDescriptorSet() const { return mDescriptorSet; }
};
NS_END
