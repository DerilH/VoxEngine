//
// Created by deril on 2/19/26.
//

#include <fstream>
#include "VoxEngine/resources/assets/AssetLoader.h"

RESOURCES_NS
    Asset *AssetLoader::fromFile(const std::filesystem::path& path) {
        std::ifstream file(path, std::ios::binary | std::ios::ate);
        if (!file)
            return nullptr;

        std::streamsize size =  std::filesystem::file_size(path);
        if (size <= 0)
            return nullptr;

        file.seekg(0, std::ios::beg);

        void* buffer = malloc(size);
        if (!buffer)
            return nullptr;

        if (!file.read(reinterpret_cast<char*>(buffer), size))
        {
            free(buffer);
            return nullptr;
        }

        auto outSize = static_cast<size_t>(size);
        return load(path.string(), buffer, outSize);
    }
NS_END
