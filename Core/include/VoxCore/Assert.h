//
// Created by deril on 2/15/26.
//

#pragma once
#include "VoxCore/Logger.h"

#ifdef VOX_DEBUG
#define VOX_ASSERT(statement, error) \
if(!(statement)) \
{ \
LOG_ERROR(error);\
throw std::runtime_error(error);\
}
#define VOX_ASSERT_PTR(ptr, error) VOX_ASSERT(ptr != nullptr, error)

#else
#define VOX_ASSERT(statement, error)
#define VOX_ASSERT_PTR(pData, error)
#endif


#define VOX_CHECK_FMT(statement, error, ...) \
if(!(statement)) \
{                                   \
LOG_ERROR(error, __VA_ARGS__)\
throw std::runtime_error(error);\
}

#define VOX_CHECK(statement, error) \
if(!(statement)) \
{                                   \
LOG_ERROR("{}", error)\
throw std::runtime_error(error);\
}

#define VOX_VERIFY(statement, error) \
if(!(statement)) \
{ \
LOG_WARN("{}", error)\
}


#define VOX_NO_IMPL(name) VOX_CHECK(false, "Called unimplemented feature: " name)

#ifdef STRICT_VULKAN
#define VK_CHECK(statement, error) \
{                                  \
VkResult r = (statement);            \
if(r != VK_SUCCESS) {              \
    LOG_ERROR("{}: {}", error, (uint32_t)r)\
    throw std::runtime_error(error);\
}                                   \
}
#else
#define VK_CHECK(statement, error) \
{                                  \
VkResult r = (statement);            \
if(r != VK_SUCCESS) {              \
LOG_ERROR("{}: {}", error, (uint32_t)r)\
}                                   \
}
#endif
