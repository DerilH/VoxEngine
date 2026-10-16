#pragma once
#include <VoxCore/Pointers.h>
#include <VoxCore/containers/Containers.h>

#include "state/PipelineStateDesc.h"

RENDER_NS
    struct PipelineVariant {
    RenderingState renderingStateHash;
    PipelineStateDesc pipeline;
};
class MaterialPipelines {
    Vector<PipelineVariant> mVariants;
public:
    explicit MaterialPipelines(Vector<PipelineVariant> variants) :  mVariants(std::move(variants)) {
    }

    ConstRef<PipelineStateDesc> find(const RenderingState& state) const {
        const uint64_t hash = std::hash<RenderingState>()(state);
        for (const auto&[renderingState, pipeline] : mVariants) {
            if (std::hash<RenderingState>()(renderingState) == hash) return &pipeline;
        }
        return nullptr;
    }
    void insert(PipelineVariant variant) {
        mVariants.emplace_back(variant);
    }
};
NS_END
