//
// Created by deril on 2/19/26.
//

#include <VoxEngine/resources/assets/TextureLoader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <TextureAsset.h>

#include "stb_image.h"
#include "../../../Define.h"
ASSETS_NS
    Asset *TextureLoader::load(std::string path, ArrayView<void> data) {
        int width, height, channels;
        unsigned char *img_data = stbi_load_from_memory(static_cast<stbi_uc const *>(data.pData), static_cast<int>(data.size()), &width, &height, &channels, STBI_rgb_alpha);
        VOX_CHECK_FMT(img_data, "Cannot read image {}", path);
        const size_t size = width * height * 4;
        return new TextureAsset(path, ArrayView<void>(img_data, size), Extent(width,height));
    }

NS_END
