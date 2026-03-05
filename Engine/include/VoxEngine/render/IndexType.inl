#pragma once
#include <cstdint>
#include "VoxCore/Define.h"
#include "Enums.h"

inline uint8_t GetIndexSize(IndexType type)
{
	switch(type) {
        case IndexType::UINT8: return 1;
		case IndexType::UINT16: return 2;
		case IndexType::UINT32: return 4;
	}
}