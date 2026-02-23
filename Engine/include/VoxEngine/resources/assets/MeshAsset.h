//
// Created by deril on 2/19/26.
//

#pragma once


#include "Asset.h"
#include <glm/glm.hpp>

RESOURCES_NS
    class MeshAsset : public Asset {
        std::vector<glm::vec3> mVertices;
        std::vector<uint32_t> mIndices;
        std::vector<glm::vec3> mNormals;
        std::vector<glm::vec2> mUVs;
    public:
        const std::vector<glm::vec3> &getVertices() const;

        const std::vector<uint32_t> &getIndices() const;

        const std::vector<glm::vec3> &getNormals() const;

        const std::vector<glm::vec2> &getUVs() const;

        AssetType type() override;

        MeshAsset(std::string path, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals, std::vector<glm::vec2> &uv, std::vector<uint32_t> &indices);

        ~MeshAsset() override;
    };
NS_END
