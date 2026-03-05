//
// Created by deril on 3/4/26.
//
#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/RenderResource.h"
#include "VertexLayout.h"
#include "UniformLayout.h"
#include "VoxEngine/render/Enums.h"
#include "VoxCore/containers/Containers.h"

RENDER_NS
class Shader : public RenderResource   {

    VertexLayout mVertexLayout;
    UniformLayout mUniformLayout;
    ShaderStage mStage;
    InternedString mEntryPoint;

    explicit Shader(ShaderStage stage, VertexLayout vertexLayout, UniformLayout uniformLayout, InternedString entryPoint = "main") : mStage(stage), mVertexLayout(vertexLayout), mUniformLayout(uniformLayout), mEntryPoint(entryPoint) {
    }
public:
    inline VertexLayout getVertexLayout() const { return mVertexLayout; }
    inline UniformLayout getUniformLayout() const { return mUniformLayout; }
    inline ShaderStage getStage() const { return mStage; }
    inline InternedString getEntryPoint() const { return mEntryPoint; }
};
NS_END
