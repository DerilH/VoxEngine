//
// Created by deril on 2/19/26.
//

#include "MeshAsset.h"
#include <utility>

ASSETS_NS
    MeshAsset::MeshAsset(InternedString path, const Vector<char8_t>& interleaved, std::vector<glm::vec3>& vertices, std::vector<glm::vec3>& normals, std::vector<uint32_t>& indices)
        : Asset(std::move(path), nullptr, 0), mVertices(std::move(vertices)), mInterleaved(std::move(interleaved)), mIndices(std::move(indices)), mNormals(std::move(normals)) {
    }

    MeshAsset::~MeshAsset() = default;
NS_END
