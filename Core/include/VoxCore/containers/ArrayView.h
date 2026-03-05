//
// Created by deril on 2/21/26.
//

#pragma once
#include "xxhash.h"

VOX_NS
template<typename PointerType>
class ArrayView {
    size_t mSize;
public:
    PointerType* pData;
    ArrayView(PointerType* ptr, size_t size) : pData(ptr), mSize(size) {
        VOX_ASSERT_PTR(ptr, "Buffer pointer is nullptr")
    }

    ArrayView(std::initializer_list<PointerType>&& list) : mSize(list.size()), pData((PointerType*) data(list))
    {
    }


    PointerType& operator[](size_t pos) {
        return pData[pos];
    }

    const PointerType& operator[](size_t pos) const {
        return pData[pos];
    }

    size_t sizeInBytes() const {
        return elementSize() * mSize;
    }

    constexpr size_t elementSize() const{
        return sizeof(PointerType);
    }

    const inline bool empty() const {return mSize == 0;}
    const inline uint32_t size() const {return mSize;}

    inline static ArrayView<PointerType> Empty() {
        ArrayView<PointerType> bf = ArrayView<PointerType>((PointerType*)1, 0);
        bf.pData = nullptr;
        return bf;
    }
};
NS_END

namespace std {
    template<typename Type>
    struct hash<Vox::ArrayView<Type>> {
        inline uint64_t operator()(const Vox::ArrayView<Type>& s) const noexcept {
            XXH64_state_t* state = XXH64_createState();
            XXH64_reset(state, 0);

            uint64_t ptr = (uint64_t)s.pData;
            XXH64_update(state, &ptr, sizeof(uint64_t));
            XXH64_update(state, s.size(), sizeof(s.size()));
            uint64_t h = XXH64_digest(state);
            XXH64_freeState(state);
            return h;
        }
    };
}