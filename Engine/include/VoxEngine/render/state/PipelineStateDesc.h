//
// Created by deril on 3/1/26.
//

#pragma once

#include <cstddef>
#include <functional>
#include <type_traits>
#include <utility>
#include "VoxCore/Define.h"
#include "ShaderState.h"
#include "BlendState.h"
#include "RasterizerState.h"
#include "MsaaState.h"
#include "RenderingState.h"

RENDER_NS
    class PipelineStateDesc {
        ShaderState mShader;
        RasterizerState mRasterizer;
        PrimitiveTopology mTopology;
        MSAAState mMsaa;
        RenderingState mRenderingState;
        mutable size_t mCachedHash = 0;
        ArrayView<BlendState> mBlend;

    public:
        PipelineStateDesc() = delete;

        explicit PipelineStateDesc(ShaderState shader, ArrayView<BlendState> blend, RasterizerState rasterizer, PrimitiveTopology topology, MSAAState msaa, RenderingState rendering) : mShader(std::move(shader)), mBlend(blend), mRasterizer(rasterizer), mTopology(topology), mMsaa(msaa), mRenderingState(rendering) {
        }

        inline const ShaderState& getShader() const { return mShader; }

        inline const ArrayView<BlendState> getBlend() const { return mBlend; }

        inline const RasterizerState& getRasterizer() const { return mRasterizer; }

        inline const MSAAState& getMSAA() const { return mMsaa; }

        inline PrimitiveTopology getTopology() const { return mTopology; }

        inline const RenderingState& getRenderingState() const { return mRenderingState; }

        size_t hash() const;

        bool operator==(const PipelineStateDesc& other) const;

        PipelineStateDesc(const PipelineStateDesc& desc);
    };
NS_END

namespace std {
    template<>
    struct hash<Vox::Render::PipelineStateDesc> {
        inline size_t operator()(const Vox::Render::PipelineStateDesc& s) const noexcept {
            return s.hash();
        }
    };
}
