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

        return std::move(vec);
    }

    std::vector<glm::vec2> readVec2Array(aiVector3D *vertices, size_t count) {
        std::vector<glm::vec2> vec(count);
        for (int i = 0; i < count; ++i) {
            vec.at(i) = glm::vec2(vertices[i].x, vertices[i].y);
        }

        return std::move(vec);
    }

    std::vector<uint32_t> readIndices(aiFace *faces, size_t count) {
        std::vector<uint32_t> vec;
        vec.reserve(count * 3);
        for (int i = 0; i < count; ++i) {
            aiFace &face = faces[i];
            for (int j = 0; j < face.mNumIndices; ++j) {
                vec.push_back(face.mIndices[j]);
            }
        }
        return vec;
    }

    Asset *FbxLoader::load(std::string path, void *data, size_t dataSize) {
        Assimp::Importer importer{};

        const aiScene *scene = importer.ReadFileFromMemory(data, dataSize, aiPostProcessSteps::aiProcess_Triangulate | aiPostProcessSteps::aiProcess_FlipUVs, "fbx");
        auto **nested = static_cast<Asset **>(malloc(scene->mNumMeshes * sizeof(MeshAsset)));
        LOG_VERBOSE("Loading model {}", path);
        LOG_VERBOSE("Loading {} meshes", scene->mNumMeshes);

        for (int i = 0; i < scene->mNumMeshes; i++) {
            aiMesh *mesh = scene->mMeshes[i];
            auto vertices = readVec3Array(mesh->mVertices, mesh->mNumVertices);
            auto normals = readVec3Array(mesh->mNormals, mesh->mNumVertices);
            auto uvs = readVec2Array(mesh->mTextureCoords[0], mesh->mNumVertices);
            auto indices = readIndices(mesh->mFaces, mesh->mNumFaces);
            nested[i] = new MeshAsset(path + "/" + mesh->mName.C_Str(), vertices, normals, uvs, indices);
            LOG_VERBOSE("Mesh loaded {}", nested[i]->getPath());
        }
        return new ModelAsset(path, nested, scene->mNumMeshes);
    }

NS_END
