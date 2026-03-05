#include "VoxEngine/render/state/ShaderState.h"

RENDER_NS

ShaderState::Builder::Builder() = default;

ShaderState::Builder &ShaderState::Builder::shader(ShaderRef shader) & {
    mShaders.push_back(shader);
    return *this;
}

ShaderState::Builder &ShaderState::Builder::shaders(const Vector<ShaderRef> &shaders) & {
    mShaders.insert(mShaders.end(), shaders.begin(), shaders.end());
    return *this;
}

ShaderState ShaderState::Builder::build() const & {
    ShaderState state;
    state.mShaders = mShaders;
    return state;
}

ShaderState::Builder ShaderState::GetBuilder() {
    return {};
}
NS_END
