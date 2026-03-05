//
// Created by deril on 2/25/26.
//

#pragma once

#include "RenderResource.h"
#include "VoxEngine/render/Enums.h"
#include "VoxCore/math/Extent.h"

RENDER_NS
    class Texture : public RenderResource {

    protected:
        Format mFormat;
        Extent mExtent;

        explicit Texture(Format format, Extent extent) : mFormat(format), mExtent(extent) {
        }
    public:

        NO_COPY_MOVE_DEFAULT(Texture);
        virtual ~Texture() = 0;

        inline Format getFormat() const {return mFormat;}
        inline Extent getExtent() const {return mExtent;}
    };
NS_END
