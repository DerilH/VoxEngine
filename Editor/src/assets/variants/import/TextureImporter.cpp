//
// Created by deril on 2/19/26.
//

#include "TextureImporter.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "../TextureAsset.h"
#include "../../../Define.h"

ASSETS_NS
    Ref<Asset> TextureImporter::load(InternedString path, ArrayView<void> data) {
        int width = 0, height = 0, channels = 0;
        unsigned char *img_data = stbi_load_from_memory(
            static_cast<stbi_uc const *>(data.pData),
            static_cast<int>(data.size()),
            &width, &height, &channels,
            STBI_rgb_alpha
        );
        VOX_CHECK_FMT(img_data != nullptr, "Cannot read image {}", path.c_str());
        const size_t size = static_cast<size_t>(width) * height * 4;
        return new TextureAsset(path, ArrayView<void>(img_data, size), Extent(width, height));
    }
NS_END
