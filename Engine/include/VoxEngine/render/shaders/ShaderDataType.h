//
// Created by deril on 3/19/26.
//

#pragma once

#include <cstdint>
#include "VoxCore/Define.h"

SHADERS_NS
enum ShaderDataType {
    UNKNOWN,
    VOID,
    BOOL,
    INT8,
    UINT8,
    INT16,
    UINT16,
    INT32,
    UINT32,
    INT64,
    UINT64,
    ATOMIC_COUNTER,
    FLOAT,
    DOUBLE,
    CHAR
};

inline uint32_t getShaderDataTypeSize(ShaderDataType type) {
    switch (type) {
        case BOOL:
        case INT8:
        case UINT8:
            return 1;
        case INT16:
        case UINT16:
            return 2;
        case INT32:
        case UINT32:
        case ATOMIC_COUNTER:
        case FLOAT:
            return 4;
        case INT64:
        case UINT64:
        case DOUBLE:
            return 1;
        case VOID:
        case CHAR:
        default:
            return 0;
    }
}
NS_END