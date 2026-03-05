//
// Created by deril on 2/25/26.
//

#pragma once

#include "VoxCore/containers/Containers.h"
#include "VoxEngine/render/Texture.h"
#include "VoxCore/math/Extent3D.h"
#include "VoxEngine/render/Enums.h"
#include "VoxEngine/render/CommandBuffer.h"
#include "VoxEngine/render/passes/RenderPass.h"
#include "GraphTexture.h"
#include <VoxCore/containers/Graph.h>

RENDER_NS
    class RenderGraph {
        Vector<GraphTextureRef> mTextures;
        HashMap<InternedString, GraphTextureRef> mSlots;

        Vector<RenderPassRef> mPasses;
        Vector<RenderPassRef> mEntryNodes;
        HashMap<AttachmentDesc, Vector<RenderPassRef>> mReadDeps;
        HashMap<AttachmentDesc, Vector<RenderPassRef>> mWriteDeps;

        bool mDirty = true;
    public:
        explicit RenderGraph() = default;
        NO_COPY_MOVE(RenderGraph);

        void addPass(RenderPass* pass);

        GraphTextureRef createTexture(InternedString slot = "");

        GraphTextureRef getTexture(InternedString slot);


        const Vector<RenderPassRef>& compile(RenderTargetRef endTarget);

        void execute(CommandBufferRef cmdBuffer, RenderTargetRef target);
    };
NS_END
