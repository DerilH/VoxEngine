//
// Created by deril on 2/25/26.
//

#pragma once

#include "VoxCore/containers/Containers.h"
#include "TextureHandle.h"
#include "VoxEngine/render/vulkan/passes/RenderPass.h"
#include "VoxEngine/render/vulkan/VulkanRenderTarget.h"
#include "VoxEngine/render/Texture.h"
#include "VoxEngine/render/vulkan/CommandBuffer.h"
#include "VoxCore/math/Extent3D.h"
#include "VoxEngine/render/Enums.h"
#include <VoxCore/containers/Graph.h>

RENDER_NS
    class LogicalDevice;
    class RenderGraph {
        const Vulkan::LogicalDevice& mDevice;
        Vector<TextureHandle> mTextures;
        HashMap<InternedString, TextureHandle> mSlots;

        Vector<Vulkan::RenderPass*> mPasses;
        Vector<Vulkan::RenderPass*> mEntryNodes{};
        HashMap<AttachmentDesc, Vector<Vulkan::RenderPass*>> mReadDeps{};
        HashMap<AttachmentDesc, Vector<Vulkan::RenderPass*>> mWriteDeps{};

        bool mDirty = true;
    private:
        TextureHandle createTextureHandle(InternedString slot = "");
    public:
        explicit RenderGraph(const Vulkan::LogicalDevice& device);
        NO_COPY_MOVE_DEFAULT(RenderGraph);

        void addPass(Vulkan::RenderPass *pass);

        TextureHandle createTexture(InternedString slot = "");
        const TextureHandle* getTextureHandle(InternedString slot);

        const Vector<Vulkan::RenderPass*>& compile(const RenderTarget* endTarget);
        void execute(Vulkan::CommandBuffer& cmdBuffer, const RenderTarget& target);
    };
NS_END
