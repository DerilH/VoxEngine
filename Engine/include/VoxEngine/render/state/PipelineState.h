//
// Created by deril on 3/4/26.
//

#pragma once

#include <utility>
#include <algorithm>

#include "VoxCore/Define.h"
#include <VoxCore/render/Enums.h>
#include "PipelineStateDesc.h"

RENDER_NS
    struct DescriptorBindingInfo {
        uint32_t binding = 0;
        ShaderResourceType resourceType = ShaderResourceType::UNIFORM_BUFFER;
        uint32_t count = 1;
        uint32_t stageFlags = 0;
        size_t size = 0;
        InternedString name;
    };

    struct DescriptorSetInfo {
        uint32_t set = 0;
        Vector<DescriptorBindingInfo> bindings;
    };

    class PipelineState {
        friend struct std::hash<PipelineState>;
        friend struct std::hash<PipelineStateRef>;
    protected:
        PipelineStateDesc mDesc;
        Vector<UniformBufferRef> mUniformBuffers;
        Vector<DescriptorSetInfo> mDescriptorSetsInfo;

    public:
        virtual ~PipelineState() = default;

        explicit PipelineState(const PipelineStateDesc &desc, Vector<DescriptorSetInfo> descriptorSetsInfo = {})
            : mDesc(desc), mDescriptorSetsInfo(std::move(descriptorSetsInfo)) {
        }

        inline const PipelineStateDesc& getDesc() const {
            return mDesc;
        }

        inline const Vector<DescriptorSetInfo>& getDescriptorSetsInfo() const {
            return mDescriptorSetsInfo;
        }

        inline const DescriptorSetInfo* getDescriptorSetInfo(uint32_t set) const {
            for (const auto& s : mDescriptorSetsInfo) {
                if (s.set == set) return &s;
            }
            return nullptr;
        }

        inline bool hasDescriptorSet(uint32_t set) const {
            return getDescriptorSetInfo(set) != nullptr;
        }

        virtual void bind(CommandBufferRef cmdBuffer) = 0;
    };

NS_END

namespace std {
    template<>
    struct hash<Vox::Render::PipelineState> {
        inline size_t operator()(Vox::Render::PipelineState &s) const noexcept {
            return s.getDesc().hash();
        }
    };

    template<>
    struct hash<Vox::Render::PipelineStateRef> {
        inline size_t operator()(Vox::Render::PipelineStateRef &s) const noexcept {
            return s->getDesc().hash();
        }
    };
}
