//
// Created by deril on 2/28/26.
//

#pragma once


#include "VoxCore/Define.h"
#include "Texture.h"
#include "VoxCore/containers/Containers.h"
#include "VoxEngine/render/graph/TextureHandle.h"

RENDER_NS
class RenderContext {
    const Vector<Texture*>& mTextures;
public:
    explicit RenderContext(const Vector<Texture*>& textures) : mTextures(textures){}
    inline Texture* getTexture(TextureHandle& handle) const{
        return mTextures[handle.id];
    }
};
NS_END
