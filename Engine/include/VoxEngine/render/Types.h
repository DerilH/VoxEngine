//
// Created by deril on 3/3/26.
//

#pragma once

RENDER_NS
    class Texture;
    class CommandBuffer;
    class RenderTarget;
    class GraphTexture;
    class RenderBuffer;
    class IndexBuffer;
    class VertexBuffer;
    class RenderPass;
    class RenderPass;
    class Shader;
    class Device;
    class CommandPool;
    class PipelineState;
    class UniformBuffer;

    using TextureRef = Texture*;
    using CommandBufferRef = CommandBuffer*;
    using PipelineStateRef = PipelineState*;
    using RenderTargetRef = RenderTarget*;
    using GraphTextureRef = GraphTexture*;
    using RenderBufferRef = RenderBuffer*;
    using IndexBufferRef = IndexBuffer*;
    using VertexBufferRef = VertexBuffer*;
    using UniformBufferRef = UniformBuffer*;
    using RenderPassRef = RenderPass*;
    using ShaderRef = Shader*;
    using DeviceRef = Device*;
    using CommandPoolRef = CommandPool*;
NS_END
