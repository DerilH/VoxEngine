//
// Created by deril on 2/23/26.
//

#pragma once


#include "VulkanObject.h"
#include "VoxEngine/render/CommandBuffer.h"

VULKAN_NS
    class VulkanCommandPool;

    class VulkanCommandBuffer : public CommandBuffer, public VulkanObject<VkCommandBuffer> {
        friend VulkanCommandPool;

    private:
        VulkanCommandBuffer(VkCommandBuffer handle) : VulkanObject(handle) {}

    public:
        void reset() override;

        void begin(int flags);
        void begin() override;

        void end() override;

        void setViewportState(float x, float y, Extent extent) override;

        void setScissor(int x, int y, Extent extent) override;

        void beginDrawingTarget(RenderTargetRef target) override;

        void endDrawingTarget(RenderTargetRef target) override;

        void setBarriers(ArrayView<PassTransition> transitions, ArrayView<TextureRef> textures) override;

        void beginRenderPass(ArrayView<AttachmentDesc> attachments, Extent size, bool clear = true) override;

        void endRenderPass() override;

        void bindPipelineState(PipelineStateRef state) override;

        void drawIndexed(uint32_t indexCount) override;

        void draw(uint32_t vertexCount) override;

        void bindIndexBuffer(IndexBufferRef buffer) override;

        void bindVertexBuffer(VertexBufferRef buffer) override;

        void bindUniformBuffer(UniformBufferRef buffer) override;

        VkPipelineLayout getCurrentLayout() const { return mCurrentLayout; }
    private:
        VkPipelineLayout mCurrentLayout = VK_NULL_HANDLE;
    public:
        NO_COPY_MOVE_DEFAULT(VulkanCommandBuffer)

    };
NS_END
