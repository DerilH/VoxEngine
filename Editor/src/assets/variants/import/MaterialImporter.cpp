#include "MaterialImporter.h"
#include <yyjson.h>
#include <magic_enum/magic_enum.hpp>
#include <VoxCore/render/Enums.h>
#include "../MaterialAsset.h"

ASSETS_NS

    inline const char *readStr(yyjson_val *root, const char *name) {
        auto obj = yyjson_obj_get(root, name);
        auto str = yyjson_get_str(obj);
        VOX_CHECK_FMT(str != nullptr, "Expected string for: {} property", name);
        return str;
    }

    template<typename Enum>
    inline Enum castEnum(const char *name) {
        auto e = magic_enum::enum_cast<Enum>(name);
        VOX_CHECK_FMT(e.has_value(), "Invalid value: {} for enum: {}", name, magic_enum::enum_type_name<Enum>());
        return *e;
    }

    template<typename Enum>
    inline Enum readEnum(yyjson_val *root, const char *name) {
        auto str = readStr(root, name);
        return castEnum<Enum>(str);
    }

    static void readShadersMap(yyjson_val *root, Vox::HashMap<Vox::Render::ShaderStage, Vox::InternedString> &out_shaders) {
        const auto obj = yyjson_obj_get(root, "shaders");
        VOX_CHECK(obj != nullptr, "Expected shaders property to be a json object");

        yyjson_val *key, *val;
        yyjson_obj_iter iter = yyjson_obj_iter_with(obj);

        while ((key = yyjson_obj_iter_next(&iter))) {
            val = yyjson_obj_iter_get_val(key);
            const auto stageStr = yyjson_get_str(key);

            auto stage_enum = castEnum<Vox::Render::ShaderStage>(stageStr);
            auto shaderPath = yyjson_get_str(val);
            VOX_CHECK(shaderPath != nullptr, "Expected string for shader path");
            std::string_view val_str(shaderPath, yyjson_get_len(val));
            out_shaders[stage_enum] = Vox::InternedString(val_str);
        }
    }

    Ref<Asset> MaterialImporter::load(InternedString path, ArrayView<void> data) {
        LOG_VERBOSE("Loading material {}", path.c_str());
        yyjson_read_err err;
        yyjson_doc *doc = yyjson_read_opts(static_cast<char *>(data.pData), data.size(), 0, nullptr, &err);
        VOX_CHECK_FMT(doc != nullptr, "Failed to parse material JSON: {}", err.msg);

        yyjson_val *root = yyjson_doc_get_root(doc);

        HashMap<Render::ShaderStage, InternedString> shaders;
        readShadersMap(root, shaders);
        auto polygonMode = readEnum<Render::PolygonMode>(root, "polygonMode");
        auto cullMode = readEnum<Render::CullMode>(root, "cullMode");
        auto topology = readEnum<Render::PrimitiveTopology>(root, "topology");

        bool depthTest = true;
        bool depthWrite = true;
        auto depthCompareOp = Render::CompareOp::LESS_OR_EQUAL;

        auto dtVal = yyjson_obj_get(root, "depthTest");
        if (dtVal && yyjson_is_bool(dtVal)) {
            depthTest = yyjson_get_bool(dtVal);
        }

        auto dwVal = yyjson_obj_get(root, "depthWrite");
        if (dwVal && yyjson_is_bool(dwVal)) {
            depthWrite = yyjson_get_bool(dwVal);
        }

        auto dcoVal = yyjson_obj_get(root, "depthCompareOp");
        if (dcoVal && yyjson_is_str(dcoVal)) {
            depthCompareOp = castEnum<Render::CompareOp>(yyjson_get_str(dcoVal));
        }

        InternedString texturePath;
        auto texVal = yyjson_obj_get(root, "texture");
        if (texVal && yyjson_is_str(texVal)) {
            texturePath = InternedString(yyjson_get_str(texVal));
        } else {
            auto albVal = yyjson_obj_get(root, "albedo");
            if (albVal && yyjson_is_str(albVal)) {
                texturePath = InternedString(yyjson_get_str(albVal));
            }
        }

        yyjson_doc_free(doc);
        return new MaterialAsset(path, shaders, polygonMode, cullMode, topology, depthTest, depthWrite, depthCompareOp, texturePath);
    }
NS_END
