//
// Created by deril on 10/5/26.
//

#include "VoxEngine/resources/assets/MaterialWriter.h"
#include <yyjson.h>
#include <fstream>
#include <VoxCore/Assert.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>
#include <magic_enum/magic_enum.hpp>

void Vox::Resources::MaterialWriter::writeToFile(const std::filesystem::path &path, Asset* asset) {
    VOX_CHECK(asset, "Asset pointer cannot be null");
    auto* material = static_cast<MaterialAsset*>(asset);
    VOX_CHECK_FMT(material, "Asset at path '{}' is not a MaterialAsset", path.string().c_str());

    yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val *root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    yyjson_mut_val *shadersObj = yyjson_mut_obj(doc);
    for (const auto& [stage, shaderPath] : material->shaders) {
        auto stageName = magic_enum::enum_name(stage);
        VOX_CHECK_FMT(!stageName.empty(), "Failed to stringify ShaderStage enum value: {}", static_cast<int>(stage));

        yyjson_mut_obj_add_strn(doc, shadersObj, stageName.data(), shaderPath.c_str(), shaderPath.size());
    }
    yyjson_mut_obj_add_val(doc, root, "shaders", shadersObj);

    auto writeEnumProp = [&](const char* propName, auto enumValue) {
        auto enumName = magic_enum::enum_name(enumValue);
        VOX_CHECK_FMT(!enumName.empty(), "Failed to stringify enum property '{}'", propName);
        yyjson_mut_obj_add_strn(doc, root, propName, enumName.data(), enumName.size());
    };

    writeEnumProp("polygonMode", material->polygonMode);
    writeEnumProp("cullMode", material->cullMode);
    writeEnumProp("topology", material->topology);

    yyjson_write_err writeErr;
    size_t jsonSize = 0;
    char *jsonStr = yyjson_mut_write_opts(
        doc,
        YYJSON_WRITE_PRETTY | YYJSON_WRITE_ESCAPE_UNICODE,
        nullptr,
        &jsonSize,
        &writeErr
    );

    VOX_CHECK_FMT(jsonStr, "Failed to write JSON for material '{}': {}", path.string().c_str(), writeErr.msg);

    if (path.has_parent_path()) {
        std::filesystem::create_directories(path.parent_path());
    }

    std::ofstream outFile(path, std::ios::out | std::ios::binary);
    VOX_CHECK_FMT(outFile.is_open(), "Failed to open file for writing: {}", path.string().c_str());

    outFile.write(jsonStr, jsonSize);
    outFile.close();

    free(jsonStr);
    yyjson_mut_doc_free(doc);
    LOG_VERBOSE("Material saved to {}", path.string());
}
