#pragma once

#include <VoxEngine/render/TextureRenderTarget.h>

#include "VulkanDevice.h"
#include "VulkanResourceCast.h"
#include "VoxCore/Define.h"
#include "VoxCore/Pointers.h"
#include "VoxEngine/render/CommandBuffer.h"

VULKAN_NS
    class VulkanTextureRenderTarget : public TextureRenderTarget {
    protected:
        Ref<VulkanDevice> mDevice;
        Ref<CommandBuffer> mCmdBuffer;

    public:
        VulkanTextureRenderTarget(const Ref<Texture> texture, const Ref<VulkanDevice> device) : TextureRenderTarget(texture), mDevice(device), mCmdBuffer(device->getCmdPool(QueueType::GRAPHICS_QUEUE).allocBuffer()) {
        }

        int32_t beginFrame() override {
            getCmdBuffer()->reset();
            getCmdBuffer()->begin();
            return 0;
        }

        void endFrame() override {
            getCmdBuffer()->end();
            mDevice->getQueue(GRAPHICS_QUEUE).submit({ResourceCast(getCmdBuffer())->getHandle()}, {}, {}, {}, nullptr);
        }

        void resize(Extent extent) override {
        }

        Ref<CommandBuffer> getCmdBuffer() const override {
            return mCmdBuffer;
        }

    };

NS_END
