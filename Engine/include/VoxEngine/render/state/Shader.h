//
// Created by deril on 3/4/26.
//
#pragma once

#include "VoxCore/Define.h"
#include "VoxEngine/render/RenderResource.h"
#include "VertexLayout.h"
#include "UniformLayout.h"
#include "VoxCore/render/Enums.h"
#include "VoxCore/containers/Containers.h"
#include "VoxEngine/render/shaders/CompiledShader.h"

RENDER_NS
class Shader : public RenderResource   {

    VertexLayout mVertexLayout;
    UniformLayout mUniformLayout;
    InternedString mEntryPoint;
    const Shaders::CompiledShader* mCompiledShader = nullptr;
public:
    explicit Shader(const Shaders::CompiledShader* code, InternedString entryPoint = "main") : mCompiledShader(code), mEntryPoint(entryPoint) {
    }
    inline VertexLayout getVertexLayout() const { return mVertexLayout; }
    inline UniformLayout getUniformLayout() const { return mUniformLayout; }
    inline ShaderStage getStage() const { return mCompiledShader->stage; }
    inline InternedString getEntryPoint() const { return mEntryPoint; }
    inline const Shaders::CompiledShader* getCompiledShader() const { return mCompiledShader; }
};
NS_END
