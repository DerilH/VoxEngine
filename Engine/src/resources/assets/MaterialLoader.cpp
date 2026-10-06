#include "VoxEngine/resources/assets/MaterialLoader.h"
#include <yyjson.h>
#include <magic_enum/magic_enum.hpp>
#include <VoxCore/render/Enums.h>
#include <VoxEngine/resources/assets/MaterialAsset.h>

inline const char *readStr(yyjson_val *root, const char *name) {
    auto obj = yyjson_obj_get(root, name);
    auto str = yyjson_get_str(obj);
    VOX_CHECK_FMT(str, "Expected string for: {} property", name);
    return str;
}

template<typename Enum>
inline Enum castEnum(const char *name) {
    auto e = magic_enum::enum_cast<Enum>(name);
    VOX_CHECK_FMT(e, "Invalid value: {} for enum: {}", name, magic_enum::enum_type_name<Enum>())
    return *e;
}

template<typename Enum>
inline Enum readEnum(yyjson_val *root, const char *name) {
    auto str = readStr(root, name);
    return castEnum<Enum>(str);
}

void readShadersMap(yyjson_val *root, Vox::HashMap<Vox::Render::ShaderStage, Vox::InternedString> &out_shaders) {
    const auto obj = yyjson_obj_get(root, "shaders");
    VOX_CHECK(obj, "Expected shaders property to be a json object")

    yyjson_val *key, *val;
    yyjson_obj_iter iter = yyjson_obj_iter_with(obj);

    while ((key = yyjson_obj_iter_next(&iter))) {
        val = yyjson_obj_iter_get_val(key);
        const auto stageStr = yyjson_get_str(key);

        auto stage_enum = castEnum<Vox::Render::ShaderStage>(stageStr);
        auto shaderPath = yyjson_get_str(val);
        VOX_CHECK(shaderPath, "Expected string for shader path");
        std::string_view val_str(shaderPath, yyjson_get_len(val));
        out_shaders[stage_enum] = Vox::InternedString(val_str);
    }
}

Vox::Ref<Vox::Resources::Asset> Vox::Resources::MaterialLoader::load(InternedString path, ArrayView<void> data) {
    LOG_VERBOSE("Loading material {}", path);
    yyjson_read_err err;
    yyjson_doc *doc = yyjson_read_opts(static_cast<char *>(data.pData), data.size(), 0, nullptr, &err);
    VOX_CHECK(doc, err.msg)

    yyjson_val *root = yyjson_doc_get_root(doc);

    HashMap<Render::ShaderStage, InternedString> shaders;
    readShadersMap(root, shaders);
    auto polygonMode = readEnum<Render::PolygonMode>(root, "polygonMode");
    auto cullMode = readEnum<Render::CullMode>(root, "cullMode");
    auto topology = readEnum<Render::PrimitiveTopology>(root, "topology");

    yyjson_doc_free(doc);
    return new MaterialAsset(path, shaders, polygonMode, cullMode, topology);
}
