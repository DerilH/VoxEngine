//
// Created by deril on 2/23/26.
//

#pragma once

#include "VoxCore/Define.h"

VULKAN_NS
    template<typename HandleType>
    class VulkanObject {
        HandleType mHandle;
    protected:
        VulkanObject(HandleType handle) : mHandle(handle) {}

        HandleType getHandle() const { return mHandle; }

        operator HandleType() const { return mHandle; }
    };
NS_END
