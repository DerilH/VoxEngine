//
// Created by deril on 2/25/26.
//

#include "VoxEngine/render/graph/RenderGraph.h"
#include "VoxEngine/render/vulkan/LogicalDevice.h"
#include "VoxEngine/render/vulkan/VulkanTexture.h"
#include "VoxEngine/render/vulkan/CommandBuffer.h"
#include "VoxEngine/render/vulkan/VulkanTextureRenderTarget.h"
#include "VoxEngine/render/vulkan/VulkanUtil.h"

RENDER_NS
    bool dfs(Vulkan::RenderPass* v, Vector<Vulkan::RenderPass*>& adj, HashMap<Vulkan::RenderPass*, bool>& visited, HashMap<Vulkan::RenderPass*, bool>& recStack) {
        visited[v] = true;
        recStack[v] = true;

        for (auto u: v->mNext) {
            if (!visited[u] && dfs(u, adj, visited, recStack)) return true;
            else if (recStack[u]) return true; // найден цикл
        }

        recStack[v] = false;
        return false;
    }

    bool HasCycle(Vector<Vulkan::RenderPass*>& adj, int n) {
        HashMap<Vulkan::RenderPass*, bool> visited(n);
        HashMap<Vulkan::RenderPass*, bool> recStack(n);

        for (int i = 0; i < n; i++) {
            if (!visited[adj[i]] && dfs(adj[i], adj, visited, recStack)) return true;
        }
        return false;
    }

    void RenderGraph::addPass(Vulkan::RenderPass* pass) {
        mDirty = true;
        mPasses.emplace_back(pass);

        auto& reads = pass->getReads();
        if (reads.empty()) mEntryNodes.emplace_back(pass);
        else {
            for (const auto& handle: reads) {
                mReadDeps[handle].emplace_back(pass);
            }
        }

        auto& writes = pass->getWrites();
        for (const auto& handle: writes) {
            mWriteDeps[handle].emplace_back(pass);
        }
    }

    const Vector<Vulkan::RenderPass*>& RenderGraph::compile(const RenderTarget* endTarget) {
        if (!mDirty) return mEntryNodes;
//        auto vktarget = (Vulkan::VulkanRenderTarget*) endTarget;
//        for (auto& entry: mTextures) {
//            if (entry.first->mNative != nullptr) continue;
//            auto ptr = mDevice.createHeap<Vulkan::VulkanTextureRenderTarget>(VK_FORMAT_B8G8R8A8_SRGB, VkExtent3D(vktarget->getExtent().width, vktarget->getExtent().height, 1), VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
//            entry.second = ptr;
//            entry.first->mNative = ptr;
//        }

        for (const auto& pass: mPasses) {
            auto& reads = pass->getReads();
            for (const auto& handle: reads) {
                auto it = mWriteDeps.find(handle);
                if (it != mWriteDeps.end()) {
                    for (auto writingPass: it->second) {
                        writingPass->mNext.emplace(pass);
                        pass->mPrev.emplace(writingPass);
                    }
                }
            }

            auto& writes = pass->getWrites();
            for (const auto& handle: writes) {
                auto it = mReadDeps.find(handle);
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

    TextureHandle Vox::Render::RenderGraph::createTexture(InternedString slot) {
        mDirty = true;
        TextureHandle handle(mTextures.size(), 1);
        mTextures.emplace_back(handle);
        if (!slot.empty())
            mSlots.emplace(slot, handle);
        return handle;
    }

    const TextureHandle* RenderGraph::getTextureHandle(InternedString slot) {
        auto it = mSlots.find(slot);
        if (it == mSlots.end()) return nullptr;
        else return &it->second;
    }

    RenderGraph::RenderGraph(const Vulkan::LogicalDevice& device) : mDevice(device) {
    }

    void createBarrier(const Vulkan::CommandBuffer& cmd, PassTransition transition, VkImage image) {
        VkImageMemoryBarrier2 imageBarrier{};

        imageBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;

        imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageBarrier.srcAccessMask = 0;

        imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
        imageBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

        imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        imageBarrier.image = image;
        imageBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageBarrier.subresourceRange.baseMipLevel = 0;
        imageBarrier.subresourceRange.levelCount = 1;
        imageBarrier.subresourceRange.baseArrayLayer = 0;
        imageBarrier.subresourceRange.layerCount = 1;

        switch (transition) {
            case NONE_W_ATTACHMENT:
                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.srcAccessMask = 0;

                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                break;

            case W_ATTACHMENT_R_COPY:
                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;

                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT;

                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
                break;
            case NONE_W_COPY:
                imageBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;
                imageBarrier.srcAccessMask = 0;

                imageBarrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
                imageBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;

                imageBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                imageBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        }

        VkDependencyInfo depInfo{};
        depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
        depInfo.imageMemoryBarrierCount = 1;
        depInfo.pImageMemoryBarriers = &imageBarrier;

        vkCmdPipelineBarrier2(cmd, &depInfo);
    }

    void Execute(RenderContext ctx, Vulkan::CommandBuffer& cmd, Vulkan::RenderPass* pass, const RenderTarget* target) {

        for (const auto& attachment: pass->getReads()) {
            createBarrier(cmd, attachment.transition, ((Vulkan::VulkanRenderTarget*) ctx.getTexture(attachment.textureHandle))->getImage());
        }

        for (const auto& attachment: pass->getWrites()) {
            createBarrier(cmd, attachment.transition, ((Vulkan::VulkanRenderTarget*) attachment.textureHandle->mNative)->getImage());
        }

        pass->setCmdBuffer(&cmd);
        pass->setExtent(target->getExtent());
        pass->execute(ctx);
    }

    void RenderGraph::execute(Vulkan::CommandBuffer& cmdBuffer, const RenderTarget& target) {

        auto entryPasses = compile(&target);
        RenderContext context(mTextures);
        for (const auto& entry: entryPasses) {
            Execute(context, cmdBuffer, entry, &target);
            for (const auto& pass: entry->mNext) {
                Execute(context, cmdBuffer, pass, &target);
            }
        }
    }
NS_END
