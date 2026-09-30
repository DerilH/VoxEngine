//
// Created by deril on 3/4/26.
//

#pragma once

#include <utility>

#include "VoxCore/Define.h"
#include "PipelineStateDesc.h"

RENDER_NS
    class PipelineState {
        friend struct std::hash<PipelineState>;
        friend struct std::hash<PipelineStateRef>;
        PipelineStateDesc mDesc;
        Vector<UniformBufferRef> mUniformBuffers;

    public:
        virtual ~PipelineState() = default;

        PipelineState(const PipelineStateDesc &desc) : mDesc(desc) {
        }

        virtual void bind(CommandBufferRef cmdBuffer) = 0;
    };

NS_END

namespace std {
    template<>
    struct hash<Vox::Render::PipelineState> {
        inline size_t operator()(Vox::Render::PipelineState &s) const noexcept {
            return s.mDesc.hash();
        }
    };

    template<>
    struct hash<Vox::Render::PipelineStateRef> {
        inline size_t operator()(Vox::Render::PipelineStateRef &s) const noexcept {
            return s->mDesc.hash();
        }
    };
}
