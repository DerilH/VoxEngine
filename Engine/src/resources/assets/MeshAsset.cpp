//
// Created by deril on 2/19/26.
//

#include "VoxEngine/resources/assets/MeshAsset.h"

RESOURCES_NS
    MeshAsset::~MeshAsset() = default;

    MeshAsset::MeshAsset(std::string path, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uv, std::vector<uint32_t>& indices) : Asset(std::move(path), nullptr, 0), mVertices(std::move(vertices)), mNormals(std::move(normals)), mUVs(std::move(uv)), mIndices(std::move(indices)) {

}

    const std::vector<glm::vec3> &MeshAsset::getVertices() const {
        return mVertices;
    }

    const std::vector<uint32_t> &MeshAsset::getIndices() const {
        return mIndices;
    }

    const std::vector<glm::vec3> &MeshAsset::getNormals() const {
        return mNormals;
    }

    const std::vector<glm::vec2> &MeshAsset::getUVs() const {
        return mUVs;
    }

    AssetType MeshAsset::type() {
        return AssetType::MESH;
    }
NS_END
