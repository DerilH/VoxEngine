//
// Created by deril on 3/4/26.
//

#pragma once

#include "VoxCore/Define.h"
#include "PipelineStateDesc.h"

RENDER_NS
class PipelineState {
    PipelineStateDesc mDesc;
public:
    PipelineState(PipelineStateDesc desc) : mDesc(desc) {
    }

    virtual void bind() = 0;
};
NS_END
