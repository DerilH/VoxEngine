//
// Created by deril on 2/20/26.
//

#pragma once

SERIALIZATION_NS
    struct BlobView {
        void const *ptr;
        const size_t count;
        const size_t elementSize;

        template <typename T>
        BlobView(T *ptr, size_t count = 1) : ptr(static_cast<void*>(ptr)), count(count), elementSize(sizeof(T)) {
            VOX_ASSERT(ptr != nullptr, "Block pointer is nullptr");
        }

        NO_COPY_MOVE_DEFAULT(BlobView)
    };
NS_END
