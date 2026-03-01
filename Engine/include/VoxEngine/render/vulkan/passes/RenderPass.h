//
// Created by deril on 2/13/26.
//

#pragma once

#include <cstddef>
#include <vulkan/vulkan_core.h>
#include <functional>
#include "VoxEngine/render/RenderPassType.h"

#include "VoxEngine/render/vulkan/pipeline/GraphicsPipeline.h"
#include "VoxEngine/render/RenderTarget.h"
#include "VoxEngine/render/graph/TextureHandle.h"
#include "VoxEngine/render/AttachmentDesc.h"
#include "VoxEngine/render/RenderContext.h"

VULKAN_NS
    class LogicalDevice;

    class RenderPass{
    protected:
        friend class LogicalDevice;
        const RenderPassType mType;
        const CommandBuffer* mCmdBuffer = nullptr;
        Vector<AttachmentDesc> mWrites;
        Vector<AttachmentDesc> mReads;

        Extent mExtent;

        RenderPass(RenderPassType mType, Vector<AttachmentDesc> reads, Vector<AttachmentDesc> writes);

    public:
        HashSet<RenderPass*> mNext;
        HashSet<RenderPass*> mPrev;

        virtual void execute(const RenderContext& context) = 0;

        RenderPassType getType() const;

        void setCmdBuffer(CommandBuffer* buffer);
        Vector<AttachmentDesc>& getWrites();
        Vector<AttachmentDesc>& getReads();

        void setExtent(Extent extent);
    };

NS_END