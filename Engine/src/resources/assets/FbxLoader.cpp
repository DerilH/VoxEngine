//
// Created by deril on 2/19/26.
//

#include "VoxEngine/resources/assets/FbxLoader.h"
#include "VoxEngine/resources/assets/MeshAsset.h"
#include "VoxEngine/resources/assets/ModelAsset.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <vector>

RESOURCES_NS
    std::vector<glm::vec3> readVec3Array(aiVector3D *vertices, size_t count) {
        std::vector<glm::vec3> vec(count);
        for (int i = 0; i < count; ++i) {
            vec.at(i) = glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z);
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

    std::vector<char8_t> createInterleaved(Vector<glm::vec3> vertices, Vector<glm::vec2> uv) {
        VOX_CHECK(vertices.size() == uv.size(), "Different vertices and color count");
        std::vector<char8_t> buffer;
        buffer.resize((vertices.size() + uv.size()) * sizeof(glm::vec3));
        auto* data = (float*)std::data(buffer);
        for (int i = 0, dataI = 0; i < vertices.size(); i++, dataI += 5) {
            data[dataI] = vertices[i].x;
            data[dataI + 1] = vertices[i].y;
            data[dataI + 2] = vertices[i].z;

            data[dataI + 3] = uv[i].x;
            data[dataI + 4] = uv[i].y;
        }
        return buffer;
    }

    Asset *FbxLoader::load(std::string path, ArrayView<void> data) {
        Assimp::Importer importer{};

        const aiScene *scene = importer.ReadFileFromMemory(data.pData, data.size(), aiPostProcessSteps::aiProcess_Triangulate);
        auto **nested = static_cast<Asset **>(malloc(scene->mNumMeshes * sizeof(MeshAsset)));
        LOG_VERBOSE("Loading model {}", path);
        LOG_VERBOSE("Loading {} meshes", scene->mNumMeshes);

        for (int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[i];
            auto vertices = readVec3Array(mesh->mVertices, mesh->mNumVertices);
            auto normals = readVec3Array(mesh->mNormals, mesh->mNumVertices);
            auto uvs = readVec2Array(mesh->mTextureCoords[0], mesh->mNumVertices);
            auto indices = readIndices(mesh->mFaces, mesh->mNumFaces);
            auto interleaved = createInterleaved(vertices,uvs);
            nested[i] = new MeshAsset(path + "/" + mesh->mName.C_Str(), interleaved, vertices, normals, uvs, indices);
            LOG_VERBOSE("Mesh loaded {}", nested[i]->getPath());
        }
        return new ModelAsset(path, nested, scene->mNumMeshes);
    }

NS_END
