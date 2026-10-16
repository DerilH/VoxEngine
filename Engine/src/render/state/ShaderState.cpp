#include "VoxEngine/render/state/ShaderState.h"

#include <VoxEngine/render/state/Shader.h>

RENDER_NS
    ShaderState::Builder::Builder() = default;

ShaderState::Builder &ShaderState::Builder::shader(ShaderRef shader) {
    mShaders.emplace(shader->getStage(), shader);
    return *this;
}

ShaderState::Builder &ShaderState::Builder::shaders(const Vector<ShaderRef> &shaders) {
    for (auto shader : shaders) {
        this->shader(shader);
    }
    return *this;
}

Expected<ShaderState> ShaderState::Builder::build() const & {
    VOX_EXPECT(mShaders.size() >= 2, "ShaderState must contain at least vertex and fragment shader")

    ShaderState state;
    state.mShaders = mShaders;
    return state;
}

ShaderState::Builder ShaderState::GetBuilder() {
    return {};
}
NS_END
