//
// Created by deril on 10/19/26.
//

#pragma once

#include <algorithm>
#include <vector>
#include <VoxCore/Define.h>
#include <VoxCore/Pointers.h>
#include <VoxEngine/render/RenderBackend.h>
#include <VoxEngine/render/buffers/UniformBuffer.h>

RENDER_NS

class UniformBufferAllocator;

class PooledUniformBuffer {
    Ref<UniformBuffer> mBuffer = nullptr;
    UniformBufferAllocator* mAllocator = nullptr;

public:
    PooledUniformBuffer() = default;

    PooledUniformBuffer(Ref<UniformBuffer> buffer, UniformBufferAllocator* allocator)
        : mBuffer(buffer), mAllocator(allocator) {
    }

    ~PooledUniformBuffer() {
        release();
    }

    PooledUniformBuffer(const PooledUniformBuffer&) = delete;
    PooledUniformBuffer& operator=(const PooledUniformBuffer&) = delete;

    PooledUniformBuffer(PooledUniformBuffer&& other) noexcept
        : mBuffer(other.mBuffer), mAllocator(other.mAllocator) {
        other.mBuffer = nullptr;
        other.mAllocator = nullptr;
    }

    PooledUniformBuffer& operator=(PooledUniformBuffer&& other) noexcept {
        if (this != &other) {
            release();
            mBuffer = other.mBuffer;
            mAllocator = other.mAllocator;
            other.mBuffer = nullptr;
            other.mAllocator = nullptr;
        }
        return *this;
    }

    void release();

    Ref<UniformBuffer> get() const {
        return mBuffer;
    }

    UniformBuffer* operator->() const {
        return mBuffer;
    }

    UniformBuffer& operator*() const {
        return *mBuffer;
    }

    explicit operator bool() const {
        return mBuffer != nullptr;
    }

    void write(const void* data, uint32_t size) {
        if (mBuffer) {
            mBuffer->write(data, size);
        }
    }

    void write(const void* data, uint32_t size, uint32_t offset) {
        if (mBuffer) {
            mBuffer->write(data, size, offset);
        }
    }
};

class UniformBufferAllocator {
    Ref<RenderBackend> mBackend;
    std::vector<Ref<UniformBuffer>> mFreeBuffers;
    std::vector<Ref<UniformBuffer>> mActiveBuffers;
    size_t mDefaultAlignment = 256;
    size_t mTotalAllocatedBytes = 0;

public:
    explicit UniformBufferAllocator(Ref<RenderBackend> backend, size_t alignment = 256);

    ~UniformBufferAllocator();

    void setAlignment(size_t alignment) {
        if (alignment > 0) {
            mDefaultAlignment = alignment;
        }
    }

    size_t getAlignment() const {
        return mDefaultAlignment;
    }

    Ref<UniformBuffer> allocate(size_t size);

    PooledUniformBuffer allocatePooled(size_t size) {
        return PooledUniformBuffer(allocate(size), this);
    }

    void free(Ref<UniformBuffer> buffer);

    void clearFreeBuffers();

    size_t getFreeCount() const {
        return mFreeBuffers.size();
    }

    size_t getActiveCount() const {
        return mActiveBuffers.size();
    }

    size_t getTotalAllocatedBytes() const {
        return mTotalAllocatedBytes;
    }

    size_t getFreeBytes() const;
};

inline void PooledUniformBuffer::release() {
    if (mBuffer && mAllocator) {
        mAllocator->free(mBuffer);
        mBuffer = nullptr;
        mAllocator = nullptr;
    }
}

NS_END
