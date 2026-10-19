//
// Created by deril on 10/19/26.
//

#include <VoxEngine/render/buffers/UniformBufferAllocator.h>
#include <VoxCore/Logger.h>

RENDER_NS

UniformBufferAllocator::UniformBufferAllocator(Ref<RenderBackend> backend, size_t alignment)
    : mBackend(backend), mDefaultAlignment(alignment > 0 ? alignment : 256) {
}

UniformBufferAllocator::~UniformBufferAllocator() {
    clearFreeBuffers();
}

Ref<UniformBuffer> UniformBufferAllocator::allocate(size_t size) {
    size_t alignedSize = (size + mDefaultAlignment - 1) & ~(mDefaultAlignment - 1);
    if (alignedSize == 0) {
        alignedSize = mDefaultAlignment;
    }

    // Best-fit search among free buffers
    int bestIndex = -1;
    size_t bestDiff = std::numeric_limits<size_t>::max();
    size_t maxAcceptableWaste = std::max<size_t>(alignedSize * 2, alignedSize + 2048);

    for (size_t i = 0; i < mFreeBuffers.size(); ++i) {
        size_t bufSize = mFreeBuffers[i]->sizeInBytes();
        if (bufSize >= alignedSize && bufSize <= maxAcceptableWaste) {
            size_t diff = bufSize - alignedSize;
            if (diff < bestDiff) {
                bestDiff = diff;
                bestIndex = static_cast<int>(i);
                if (diff == 0) break; // Exact match found
            }
        }
    }

    Ref<UniformBuffer> result = nullptr;
    if (bestIndex >= 0) {
        result = mFreeBuffers[bestIndex];
        mFreeBuffers.erase(mFreeBuffers.begin() + bestIndex);
    } else {
        result = mBackend->createUniformBuffer(static_cast<uint32_t>(alignedSize));
        mTotalAllocatedBytes += alignedSize;
    }

    if (result) {
        mActiveBuffers.push_back(result);
    }

    return result;
}

void UniformBufferAllocator::free(Ref<UniformBuffer> buffer) {
    if (!buffer) return;

    auto it = std::find(mActiveBuffers.begin(), mActiveBuffers.end(), buffer);
    if (it != mActiveBuffers.end()) {
        mActiveBuffers.erase(it);
    }

    auto freeIt = std::find(mFreeBuffers.begin(), mFreeBuffers.end(), buffer);
    if (freeIt == mFreeBuffers.end()) {
        mFreeBuffers.push_back(buffer);
    }
}

void UniformBufferAllocator::clearFreeBuffers() {
    for (auto& buf : mFreeBuffers) {
        mTotalAllocatedBytes = (mTotalAllocatedBytes >= buf->sizeInBytes())
            ? (mTotalAllocatedBytes - buf->sizeInBytes()) : 0;
    }
    mFreeBuffers.clear();
}

size_t UniformBufferAllocator::getFreeBytes() const {
    size_t sum = 0;
    for (const auto& buf : mFreeBuffers) {
        sum += buf->sizeInBytes();
    }
    return sum;
}

NS_END
