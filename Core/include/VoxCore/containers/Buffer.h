//
// Created by deril on 2/21/26.
//

#pragma once

VOX_NS
template<typename PointerType>
struct Buffer {
    PointerType* const pData;
    const size_t size;
    Buffer(PointerType* ptr, const size_t size) : pData(ptr), size(size) {
        VOX_ASSERT_PTR(ptr, "Buffer pointer is nullptr")
    }
    Buffer(std::initializer_list<PointerType> list) : size(list.size()), pData(data(list))
    {
    }


    Buffer() = delete;

    const PointerType& operator[](size_t pos) {
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

    NON_COPYABLE(Buffer)
};
NS_END
