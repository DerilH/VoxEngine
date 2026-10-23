//
// Created by deril on 2/20/26.
//

#pragma once
#include <VoxCore/Define.h>
#include <VoxCore/Assert.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/math/Extent.h>
#include <VoxCore/render/Enums.h>
#include "ResourceType.h"
#include "ResourceHandle.h"

RESOURCES_NS
    class TextureResource {
        Extent mExtent;
        Render::Format mFormat = Render::Format::RGBA8;
        Vector<uint8_t> mPixels;

    public:
        TextureResource() : mExtent(0, 0) {}

        TextureResource(Extent extent, Render::Format format, Vector<uint8_t> pixels)
            : mExtent(extent), mFormat(format), mPixels(std::move(pixels)) {}

        Expected<void> validate() const {
            VOX_EXPECT(mExtent.width > 0 && mExtent.height > 0, "Texture dimensions must be positive, got {}x{}", mExtent.width, mExtent.height);
            VOX_EXPECT(!mPixels.empty(), "Texture pixel data is empty");
            return {};
        }

        Extent getExtent() const noexcept { return mExtent; }
        Render::Format getFormat() const noexcept { return mFormat; }
        const Vector<uint8_t> &getPixels() const noexcept { return mPixels; }
        const uint8_t *getData() const noexcept { return mPixels.data(); }
        size_t getSize() const noexcept { return mPixels.size(); }
    };

    template<>
    struct ResourceTraits<TextureResource> {
        static constexpr ResourceType Type = ResourceType::TEXTURE;
        static constexpr const char *Name = "Texture";
    };

    using TextureHandle = ResourceHandle<TextureResource>;
NS_END
