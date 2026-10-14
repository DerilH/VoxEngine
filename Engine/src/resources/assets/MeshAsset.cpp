//
// Created by deril on 2/19/26.
//

#include <utility>

#include "VoxEngine/resources/assets/MeshAsset.h"

RESOURCES_NS
    MeshAsset::MeshAsset(InternedString path, Vector<char8_t> interleaved, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<glm::vec2>& uv, std::vector<uint32_t>& indices) : Asset(std::move(path), nullptr, 0), mInterleaved(std::move(interleaved)), mVertices(std::move(vertices)), mNormals(std::move(normals)), mUVs(std::move(uv)), mIndices(std::move(indices)) {
    }

    MeshAsset::~MeshAsset() = default;
NS_END
