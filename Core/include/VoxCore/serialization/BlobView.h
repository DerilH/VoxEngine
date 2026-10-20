#pragma once

#include <cstddef>
#include <VoxCore/Assert.h>
#include <VoxCore/Define.h>

SERIALIZATION_NS

    struct BlobView {
        void const *pData;
        const size_t count;
        const size_t elementSize;

        template <typename T>
        BlobView(const T *ptr, size_t count = 1) : pData(static_cast<const void*>(ptr)), count(count), elementSize(sizeof(T)) {
            VOX_ASSERT(ptr != nullptr, "Blob pointer is nullptr");
        }

        BlobView(const void *ptr, size_t elementSize, size_t count) : pData(ptr), count(count), elementSize(elementSize) {
            VOX_ASSERT(ptr != nullptr, "Blob pointer is nullptr");
        }

        NO_COPY_MOVE_DEFAULT(BlobView)
    };

NS_END

VOX_NS
    using BlobView = Vox::Resources::Serialization::BlobView;
NS_END
