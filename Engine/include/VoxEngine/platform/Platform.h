//
// Created by deril on 3/5/26.
//

#pragma once

#include "VoxCore/Define.h"

PLATFORM_NS
class Platform {
public:
    virtual void enableSandbox(std::filesystem::path allowedDirectory) = 0;
};
NS_END
