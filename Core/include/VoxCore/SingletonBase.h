#pragma once
#include "Define.h"

VOX_NS
    template<typename T>
    class SingletonBase {
    public:
        SingletonBase(const SingletonBase &) = delete;

        SingletonBase &operator=(const SingletonBase &) = delete;

        SingletonBase(SingletonBase &&) = delete;

        SingletonBase &operator=(SingletonBase &&) = delete;

        static T &Get() {
            static T instance;
            return instance;
        }

    protected:
        SingletonBase() = default;

        ~SingletonBase() = default;
    };
NS_END

