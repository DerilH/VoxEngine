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

    using TextureRef = Texture*;
    using CommandBufferRef = CommandBuffer*;
    using RenderTargetRef = RenderTarget*;
    using GraphTextureRef = GraphTexture*;
    using RenderBufferRef = RenderBuffer*;
    using IndexBufferRef = IndexBuffer*;
    using VertexBufferRef = VertexBuffer*;
    using RenderPassRef = RenderPass*;
    using ShaderRef = Shader*;
    using DeviceRef = Device*;

NS_END
