//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/graph/RenderGraph.h"
#include "VoxEngine/render/CommandBuffer.h"
#include "VoxEngine/render/RenderTarget.h"
#include "VoxEngine/render/RenderBackend.h"

RENDER_NS
    bool dfs(RenderPassRef v, Vector<RenderPassRef>& adj, HashMap<RenderPassRef, bool>& visited, HashMap<RenderPassRef, bool>& recStack) {
        visited[v] = true;
        recStack[v] = true;

        for (auto u: v->mNext) {
            if (!visited[u] && dfs(u, adj, visited, recStack)) return true;
            else if (recStack[u]) return true;
        }

        recStack[v] = false;
        return false;
    }

    bool HasCycle(Vector<RenderPass*>& adj, int n) {
        HashMap<RenderPass*, bool> visited(n);
        HashMap<RenderPass*, bool> recStack(n);

        for (int i = 0; i < n; i++) {
            if (!visited[adj[i]] && dfs(adj[i], adj, visited, recStack)) return true;
        }
        return false;
    }

    void RenderGraph::addPass(RenderPass* pass) {
        mDirty = true;
        mPasses.emplace_back(pass);

        auto& reads = pass->getReads();
        if (reads.empty()) mEntryNodes.emplace_back(pass);
        else {
            for (int i = 0; i < reads.size(); i++) {
                mReadDeps[reads[i]].emplace_back(pass);
            }
        }

        auto& writes = pass->getWrites();
        for (int i = 0; i < writes.size(); i++) {
            mWriteDeps[writes[i]].emplace_back(pass);
        }
    }

    const Vector<RenderPass*>& RenderGraph::compile(const RenderTargetRef endTarget) {
        if (!mDirty) return mEntryNodes;

        for (const auto& pass: mPasses) {
            auto& reads = pass->getReads();
            for (int i = 0; i < reads.size(); i++) {
                auto it = mWriteDeps.find(reads[i]);
                if (it != mWriteDeps.end()) {
                    for (auto writingPass: it->second) {
                        writingPass->mNext.emplace(pass);
                        pass->mPrev.emplace(writingPass);
                    }
                }
            }

            auto& writes = pass->getWrites();
            for (int i = 0; i < writes.size(); i++) {
                auto it = mReadDeps.find(writes[i]);
                if (it != mReadDeps.end()) {
                    for (const auto& readingPass: it->second) {
                        readingPass->mPrev.emplace(pass);
                        pass->mNext.emplace(readingPass);
                    }
                }
            }
        }

        VOX_CHECK(!HasCycle(mPasses, mPasses.size()), "Cycle found in render graph!")
        mDirty = false;
        return mEntryNodes;
    }

    GraphTextureRef Vox::Render::RenderGraph::createTexture(InternedString slot) {
        mDirty = true;
        auto tex = new GraphTexture(mTextures.size());
        mTextures.emplace_back(tex);
        if (!slot.empty())
            mSlots.emplace(slot, tex);
        return tex;
    }

    GraphTextureRef RenderGraph::getTexture(InternedString slot) {
        auto it = mSlots.find(slot);
        if (it == mSlots.end()) return nullptr;
        else return it->second;
    }

    void Execute(RenderContext context, RenderPassRef pass, RenderTargetRef viewport) {


        Vector<AttachmentDesc> attachments;
        for (int i = 0; i < pass->getReads().size(); i++) {
            attachments.emplace_back(pass->getReads()[i]);;
        }

        for (int i = 0; i < pass->getWrites().size(); i++) {
            attachments.emplace_back(pass->getWrites()[i]);
        }

        context.cmdBuffer->beginRenderPass(ArrayView(data(attachments), attachments.size()), {viewport->getSize()});
        for(int i = 0; i < pass->getReads().size(); i++) {
            AttachmentDesc attachment = pass->getReads()[i];
            context.cmdBuffer->setBarriers({attachment.transition}, {attachment.texture->getExact()});
        }

        for(int i = 0; i < pass->getWrites().size(); i++) {
            AttachmentDesc attachment = pass->getWrites()[i];
            context.cmdBuffer->setBarriers({attachment.transition}, {attachment.texture->getExact()});
        }

        pass->setExtent(viewport->getSize());
        pass->execute(context);
        context.cmdBuffer->endRenderPass();

    }

    void RenderGraph::execute(RenderContext context, const RenderTargetRef target) {
        auto entryPasses = compile(target);
        for (auto  entry: entryPasses) {
            Execute(context, entry, target);
            for (const auto pass: entry->mNext) {
                Execute(context, pass, target);
            }
        }
    }

    Vector<GraphTextureRef> RenderGraph::getTextures() {
        return mTextures;
    }

NS_END
