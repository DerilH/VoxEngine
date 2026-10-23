//
// Created by deril on 2/19/26.
//

#pragma once

#include "Asset.h"
#include <vector>
#include <glm/glm.hpp>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/containers/ArrayView.h>

ASSETS_NS
    class MeshAsset : public Asset {
        std::vector<glm::vec3> mVertices;
        Vector<char8_t> mInterleaved;
        std::vector<uint32_t> mIndices;
        std::vector<glm::vec3> mNormals;

    public:
        MeshAsset(const Vector<char8_t>& interleaved, std::vector<glm::vec3> &vertices, std::vector<glm::vec3> &normals, std::vector<uint32_t> &indices);

        ~MeshAsset() override;

        const Vector<char8_t>& getInterleaved() const {
            return mInterleaved;
        }

        const std::vector<glm::vec3> &getVertices() const {
            return mVertices;
        }

        const std::vector<uint32_t> &getIndices() const {
            return mIndices;
        }

        const std::vector<glm::vec3> &getNormals() const {
            return mNormals;
        }

        AssetType type() const override {
            return StaticType();
        }

        static AssetType StaticType() {
            return AssetType::MESH;
        }
    };

NS_END

namespace Vox::Resources {
    using MeshAsset = Vox::Editor::Assets::MeshAsset;
}
