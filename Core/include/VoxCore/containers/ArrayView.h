//
// Created by deril on 2/21/26.
//

#pragma once
#include "xxhash.h"
#include "VoxCore/containers/Containers.h"

VOX_NS
    template<typename PointerType>
    class ArrayView {
        size_t mSize;

    public:
        PointerType *pData = nullptr;

        ArrayView(PointerType *ptr, size_t size) : pData(ptr), mSize(size) {
            VOX_ASSERT_PTR(ptr, "Buffer pointer is nullptr")
        }

        ArrayView(std::initializer_list<PointerType> &&list) : mSize(list.size()) {
            auto dataPtr = data(list);
            if (dataPtr == nullptr) return;
            pData = static_cast<PointerType *>(::operator new(mSize * sizeof(PointerType)));
            memcpy(pData, dataPtr, mSize * sizeof(PointerType));
        }

        ArrayView(std::initializer_list<PointerType> &list) : mSize(list.size()) {
            auto dataPtr = data(list);
            if (dataPtr == nullptr) return;
            pData = static_cast<PointerType *>(::operator new(mSize * sizeof(PointerType)));
            memcpy(pData, dataPtr, mSize * sizeof(PointerType));
        }


        static ArrayView Copy(Vector<PointerType> &list) {
            auto dataPtr = data(list);
            if (dataPtr == nullptr) Empty();
            PointerType *pData = reinterpret_cast<PointerType *>(::operator new(list.size() * sizeof(PointerType)));
            memcpy(pData, dataPtr, list.size() * sizeof(PointerType));
            return ArrayView(pData, list.size());
        }

        PointerType &operator[](size_t pos) {
            return pData[pos];
        }

        const PointerType &operator[](size_t pos) const {
            return pData[pos];
        }

        size_t sizeInBytes() const {
            return elementSize() * mSize;
        }

        constexpr size_t elementSize() const {
            return sizeof(PointerType);
        }

        bool isNull() const { return pData == nullptr; }
        bool empty() const { return isNull() || mSize == 0; }
        uint32_t size() const { return mSize; }

        static ArrayView Empty() {
            ArrayView bf = ArrayView((PointerType *) 1, 0);
            bf.pData = nullptr;
            return bf;
        }
    };

    template<>
    class ArrayView<void> {
        size_t mSizeBytes = 0;

    public:
        void *pData = nullptr;

        ArrayView() = default;

        ArrayView(void *ptr, size_t sizeInBytes) : mSizeBytes(sizeInBytes), pData(ptr) {
        }

        template<typename T>
        ArrayView(ArrayView<T> typedView)
            : pData(typedView.pData), mSizeBytes(typedView.sizeInBytes()) {
        }

        size_t sizeInBytes() const { return mSizeBytes; }
        constexpr size_t elementSize() const { return 1; }

        bool isNull() const { return pData == nullptr; }
        bool empty() const { return isNull() || mSizeBytes == 0; }
        uint32_t size() const { return static_cast<uint32_t>(mSizeBytes); }

        static ArrayView Empty() {
            return ArrayView(nullptr, 0);
        }
    };

    template<>
    class ArrayView<const void> {
        size_t mSizeBytes = 0;

    public:
        const void *pData = nullptr;

        ArrayView() = default;

        ArrayView(const void *ptr, size_t sizeInBytes) : pData(ptr), mSizeBytes(sizeInBytes) {
        }

        template<typename T>
        ArrayView(ArrayView<T> typedView)
            : pData(typedView.pData), mSizeBytes(typedView.sizeInBytes()) {
        }

        ArrayView(ArrayView<void> voidView)
            : pData(voidView.pData), mSizeBytes(voidView.sizeInBytes()) {
        }

        size_t sizeInBytes() const { return mSizeBytes; }
        constexpr size_t elementSize() const { return 1; }

        bool isNull() const { return pData == nullptr; }
        bool empty() const { return isNull() || mSizeBytes == 0; }
        uint32_t size() const { return static_cast<uint32_t>(mSizeBytes); }

        static ArrayView<const void> Empty() {
            return ArrayView<const void>(nullptr, 0);
        }
    };

NS_END

namespace std {
    template<typename Type>
    struct hash<Vox::ArrayView<Type> > {
        inline uint64_t operator()(const Vox::ArrayView<Type> &s) const noexcept {
            XXH64_state_t *state = XXH64_createState();
            XXH64_reset(state, 0);

            uint64_t ptr = (uint64_t) s.pData;
            XXH64_update(state, &ptr, sizeof(uint64_t));
            XXH64_update(state, s.size(), sizeof(s.size()));
            uint64_t h = XXH64_digest(state);
            XXH64_freeState(state);
            return h;
        }
    };
}
