//
// Created by deril on 2/25/26.
//

#pragma once

#include <oneapi/tbb/parallel_reduce.h>

#include "VoxCore/containers/Containers.h"
#include "VoxEngine/render/Texture.h"
#include "VoxCore/math/Extent3D.h"
#include "VoxCore/render/Enums.h"
#include "VoxEngine/render/CommandBuffer.h"
#include "VoxEngine/render/passes/RenderPass.h"
#include "GraphTexture.h"
#include <VoxCore/containers/Graph.h>
#include <VoxEngine/render/RenderBackend.h>

RENDER_NS
    class RenderGraph {
        Vector<GraphTextureRef> mTextures;
        HashMap<InternedString, GraphTextureRef> mSlots;

        Vector<RenderPassRef> mPasses;
        Vector<RenderPassRef> mEntryNodes;
        HashMap<AttachmentDesc, Vector<RenderPassRef> > mReadDeps;
        HashMap<AttachmentDesc, Vector<RenderPassRef> > mWriteDeps;
        Ref<RenderBackend> mBackend;
        Extent mViewportSize{};

        bool mDirty = true;

    public:
        explicit RenderGraph(Ref<RenderBackend> renderBackend, RenderGraph *old);

        NO_COPY_MOVE(RenderGraph);

        void addPass(RenderPass *pass);

        GraphTextureRef createTexture(InternedString slot = "", Format format = {});

        GraphTextureRef importTexture(InternedString slot, const std::function<Ref<Texture>()> &provider);

        GraphTextureRef getTexture(InternedString slot);

        Vector<GraphTextureRef> getTextures() {
            return mTextures;
        }

        void setViewportSize(Extent size) {
            if (mViewportSize != size) {
                mDirty = true;
            }
            mViewportSize = size;
        }

        const Vector<RenderPassRef> &compile(RenderTargetRef endTarget);

        void execute(RenderContext context, const RenderTargetRef target);
    };

NS_END
