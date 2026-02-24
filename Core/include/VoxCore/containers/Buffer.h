//
// Created by deril on 2/21/26.
//

#pragma once

VOX_NS
template<typename PointerType>
struct Buffer {
    PointerType* pData;
    const size_t size;
    Buffer(PointerType* ptr, const size_t size) : pData(ptr), size(size) {
        VOX_ASSERT_PTR(ptr, "Buffer pointer is nullptr")
    }

    Buffer(std::initializer_list<PointerType>&& list) : size(list.size()), pData((PointerType*) data(list))
    {
    }

    Buffer() = delete;

    Buffer(const Buffer&& ref)  noexcept : pData(std::move(ref.pData)), size(ref.size) {
    };


    PointerType& operator[](size_t pos) {
        return pData[pos];
    }

    const PointerType& operator[](size_t pos) const {
        return pData[pos];
    }

    size_t sizeInBytes() const {
        return elementSize() * size;
    }

    constexpr size_t elementSize() const{
        return sizeof(PointerType);
    }

    inline static Buffer<PointerType> Empty() {
        Buffer<PointerType> bf = Buffer<PointerType>((PointerType*)1, 0);
        bf.pData = nullptr;
        return bf;
    }

    NON_COPYABLE(Buffer)
};
NS_END
