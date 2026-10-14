#pragma once
#include <VoxCore/Define.h>

VOX_NS
template <typename... Args>
class Delegate {
public:
    Delegate() = default;

    template <auto Method, typename T>
    static Delegate from(T* listener) {
        Delegate d;
        d.mListener = listener;
        d.mStub = [](void* ptr, Args... args) {
            (static_cast<T*>(ptr)->*Method)(args...);
        };
        d.mDeleter = nullptr;
        return d;
    }

    template <typename F>
    requires (!std::is_same_v<std::decay_t<F>, Delegate>)
    Delegate(F&& lambda) {
        using DecayedF = std::decay_t<F>;
        static_assert(sizeof(DecayedF) <= sizeof(m_buffer), "Delegate lambda is too large for internal buffer");

        new (m_buffer) DecayedF(std::forward<F>(lambda));

        mStub = [](void* ptr, Args... args) {
            auto& l = *reinterpret_cast<DecayedF*>(ptr);
            l(args...);
        };

        mDeleter = [](void* ptr) {
            reinterpret_cast<DecayedF*>(ptr)->~DecayedF();
        };

        mListener = m_buffer;
    }

    ~Delegate() {
        reset();
    }

    Delegate(const Delegate& other) {
        copy_from(other);
    }

    Delegate& operator=(const Delegate& other) {
        if (this != &other) {
            reset();
            copy_from(other);
        }
        return *this;
    }

    void invoke(Args... args) const {
        if (mStub && mListener) {
            mStub(mListener, args...);
        }
    }

    void operator()(Args... args) const {
        invoke(args...);
    }

private:
    void reset() {
        if (mDeleter && mListener) {
            mDeleter(mListener);
        }
        mStub = nullptr;
        mListener = nullptr;
        mDeleter = nullptr;
    }

    void copy_from(const std::decay_t<Delegate>& other) {
        mStub = other.mStub;
        mDeleter = other.mDeleter;
        if (other.mListener == other.m_buffer) {
            std::memcpy(m_buffer, other.m_buffer, sizeof(m_buffer));
            mListener = m_buffer;
        } else {
            mListener = other.mListener;
        }
    }

    using StubPtr = void(*)(void*, Args...);
    using DeleterPtr = void(*)(void*);

    void* mListener = nullptr;
    StubPtr mStub = nullptr;
    DeleterPtr mDeleter = nullptr;

    alignas(void*) char m_buffer[32];
};


NS_END
