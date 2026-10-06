#pragma once
#include "Define.h"
VOX_NS

template<typename Type>
using Ref = Type*;

template<typename Type>
using ConstRef = const Type*;


template<typename Type>
using UPtr = std::unique_ptr<Type>;

template<typename Type, typename... Args>
UPtr<Type> makeUPtr(Args&&... args) {
    return std::make_unique<Type>(std::forward<Args>(args)...);
}


NS_END