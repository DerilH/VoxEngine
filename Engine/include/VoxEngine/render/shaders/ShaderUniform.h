//
// Created by deril on 3/19/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/containers/Containers.h"
#include "ShaderDataType.h"

SHADERS_NS
struct ShaderUniform {
private:
    const uint32_t mArraySize;
public:

    struct Member {
        InternedString name;
        ShaderDataType typeId;
        uint32_t components;
        size_t offset;
        Member(const InternedString name, ShaderDataType typeId, size_t offset) : name(name), typeId(typeId), offset(offset) {}
    };
    const InternedString name;
//    const Vector<Member> members;
    const size_t size;
    const uint32_t binding;
    const uint32_t set;
    const uint32_t location;

    ShaderUniform(InternedString name, const uint32_t location, const uint32_t binding, const uint32_t set, const size_t size, const uint32_t arraySize) : mArraySize(arraySize), name(name), size(size), binding(binding), set(set), location(location) {
    }

    bool isArray() const {
        return mArraySize != 0;
    }

    uint32_t getArraySize() const {
        return mArraySize;
    }
};

NS_END
