//
// Created by deril on 3/5/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "Platform.h"

PLATFORM_NS
class LinuxPlatform : public Platform {
public:
    void enableSandbox(std::filesystem::path allowedDirectory) override;
};
NS_END
