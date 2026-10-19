//
// Created by deril on 3/19/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "VoxCore/containers/Containers.h"
#include <VoxCore/render/Enums.h>
#include "ShaderDataType.h"

SHADERS_NS
class ShaderUniform {
private:
    uint32_t mArraySize;
public:

    struct Member {
        InternedString name;
        ShaderDataType typeId;
        uint32_t components;
        size_t offset;
        Member(const InternedString name, ShaderDataType typeId, size_t offset) : name(name), typeId(typeId), offset(offset) {}
    };
    InternedString name;
//    const Vector<Member> members;
    size_t size;
    uint32_t binding;
    uint32_t set;
    uint32_t location;
    ShaderResourceType resourceType;

    ShaderUniform(InternedString name, const uint32_t location, const uint32_t binding, const uint32_t set, const size_t size, const uint32_t arraySize, const ShaderResourceType resourceType = ShaderResourceType::UNIFORM_BUFFER)
        : mArraySize(arraySize), name(name), size(size), binding(binding), set(set), location(location), resourceType(resourceType) {
    }

    bool isArray() const {
        return mArraySize != 0;
    }

    uint32_t getArraySize() const {
        return mArraySize;
    }
};

NS_END
