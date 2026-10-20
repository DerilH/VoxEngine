//
// Created by deril on 2/19/26.
//

#include "FbxImporter.h"
#include "../../../../Engine/include/VoxEngine/resources/Mesh.h"
#include "../../../../Engine/include/VoxEngine/resources/Model.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <vector>

ASSETS_NS
    std::vector<glm::vec3> readVec3Array(aiVector3D *vertices, size_t count) {
        std::vector<glm::vec3> vec(count);
        for (int i = 0; i < count; ++i) {
            vec.at(i) = glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z);
        }

        return vec;
    }

    std::vector<glm::vec4> readVec4Array(aiColor4D *vertices, size_t count) {
        std::vector<glm::vec4> vec(count);
        for (int i = 0; i < count; ++i) {
            vec.at(i) = glm::vec4(vertices[i].r, vertices[i].g, vertices[i].b, vertices[i].a);
        }

        return vec;
    }

    std::vector<glm::vec2> readVec2Array(aiVector3D *vertices, size_t count) {
        std::vector<glm::vec2> vec(count);
        for (int i = 0; i < count; ++i) {
            vec.at(i) = glm::vec2(vertices[i].x, vertices[i].y);
        }

        return vec;
    }

    std::vector<uint32_t> readIndices(aiFace *faces, size_t count) {
        std::vector<uint32_t> vec;
        vec.reserve(count * 3);
        for (int f = 0; f < count; f++) {
            const aiFace &face = faces[f];
            for (int i = 0; i < face.mNumIndices; i++) {
                vec.push_back(face.mIndices[i]);
            }
        }
        return vec;
    }

    template<typename T>
    std::vector<char8_t> createInterleaved(const Vector<glm::vec3> &vertices, const Vector<T> &attrs) {
        static_assert(std::is_same_v<typename T::value_type, float>, "Attribute must be a float glm vector");
        VOX_CHECK(vertices.size() == attrs.size(), "Different vertices and attribute count");

        constexpr size_t attrLen = T::length();
        constexpr size_t stride = (3 + attrLen) * sizeof(float);

        std::vector<char8_t> buffer(vertices.size() * stride);
        char8_t *dst = buffer.data();

        for (size_t i = 0; i < vertices.size(); ++i, dst += stride) {
            std::memcpy(dst, &vertices[i], sizeof(glm::vec3));
            std::memcpy(dst + sizeof(glm::vec3), &attrs[i], sizeof(T));
        }
        return buffer;
    }

    Asset *FbxImporter::load(std::string path, ArrayView<void> data) {
        Assimp::Importer importer{};

        const aiScene *scene = importer.ReadFileFromMemory(data.pData, data.size(), aiPostProcessSteps::aiProcess_Triangulate);
        auto **nested = static_cast<Asset **>(malloc(scene->mNumMeshes * sizeof(Mesh)));
        LOG_VERBOSE("Loading model {}", path);
        LOG_VERBOSE("Loading {} meshes", scene->mNumMeshes);

        for (int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[i];

            auto vertices = readVec3Array(mesh->mVertices, mesh->mNumVertices);
            auto normals = readVec3Array(mesh->mNormals, mesh->mNumVertices);
            auto indices = readIndices(mesh->mFaces, mesh->mNumFaces);
            auto interleaved = mesh->HasVertexColors(0) ? createInterleaved<glm::vec4>(vertices, readVec4Array(mesh->mColors[0], mesh->mNumVertices)) : createInterleaved<glm::vec2>(vertices, readVec2Array(mesh->mTextureCoords[0], mesh->mNumVertices));

            nested[i] = new Mesh(path + "/" + mesh->mName.C_Str(), interleaved, vertices, normals, indices);
            LOG_VERBOSE("Mesh loaded {}", nested[i]->getPath());
        }
        return new Model(path, nested, scene->mNumMeshes);
    }

NS_END
