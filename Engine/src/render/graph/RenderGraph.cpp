//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/graph/RenderGraph.h"
#include "VoxEngine/render/CommandBuffer.h"
#include "VoxEngine/render/RenderTarget.h"

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

    void Execute(CommandBufferRef cmd, RenderPassRef pass, RenderTargetRef viewport) {

        for(int i = 0; i < pass->getReads().size(); i++) {
            AttachmentDesc attachment = pass->getReads()[i];
            cmd->setBarrier({attachment.transition}, {attachment.texture->getExact()});
        }

        for(int i = 0; i < pass->getWrites().size(); i++) {
            AttachmentDesc attachment = pass->getWrites()[i];
            cmd->setBarrier({attachment.transition}, {attachment.texture->getExact()});
        }

        pass->setExtent(viewport->getSize());
        pass->execute(nullptr);
    }

    void RenderGraph::execute(CommandBufferRef cmdBuffer, const RenderTargetRef target) {
        auto entryPasses = compile(target);
        for (auto  entry: entryPasses) {
            Execute(cmdBuffer, entry, target);
            for (const auto pass: entry->mNext) {
                Execute(cmdBuffer, pass, target);
            }
        }
    }

//    void createBarrier(CommandBuffer& cmd, PassTransition transition, VkImage image) {
//        VkImageMemoryBarrier2 imageBarrier{};
//
//        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
//
//        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
//        imageBarrier.srcAccessMask = 0;
//
//        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
//        imageBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
//
//        imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//        imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//
//        imageBarrier.image = image;
//        imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//        imageBarrier.subresourceRange.baseMipLevel = 0;
//        imageBarrier.subresourceRange.levelCount = 1;
//        imageBarrier.subresourceRange.baseArrayLayer = 0;
//        imageBarrier.subresourceRange.layerCount = 1;
//
//        switch (transition) {
//            case NONE_W_ATTACHMENT:
//                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
//                imageBarrier.srcAccessMask = 0;
//
//                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
//                imageBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
//
//                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//                imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//                break;
//
//            case W_ATTACHMENT_R_COPY:
//                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
//                imageBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
//
//                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
//                imageBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;
//
//                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//                imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
//                break;
//            case NONE_W_COPY:
//                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
//                imageBarrier.srcAccessMask = 0;
//
//                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
//                imageBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
//
//                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
//                imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
//        }
//
//        VkDependencyInfo depInfo{};
//        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
//        depInfo.imageMemoryBarrierCount = 1;
//        depInfo.pImageMemoryBarriers = &imageBarrier;
//
//        vkCmdPipelineBarrier2(cmd, &depInfo);
//    }
NS_END
