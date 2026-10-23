#include "MaterialImporter.h"
#include <yyjson.h>
#include <magic_enum/magic_enum.hpp>
#include <VoxCore/render/Enums.h>
#include "../MaterialAsset.h"

ASSETS_NS

    // inline const char *readStr(yyjson_val *root, const char *name) {
    //     auto obj = yyjson_obj_get(root, name);
    //     auto str = yyjson_get_str(obj);
    //     VOX_CHECK_FMT(str != nullptr, "Expected string for: {} property", name);
    //     return str;
    // }
    //
    // template<typename Enum>
    // inline Enum castEnum(const char *name) {
    //     auto e = magic_enum::enum_cast<Enum>(name);
    //     VOX_CHECK_FMT(e.has_value(), "Invalid value: {} for enum: {}", name, magic_enum::enum_type_name<Enum>());
    //     return *e;
    // }
    //
    // template<typename Enum>
    // inline Enum readEnum(yyjson_val *root, const char *name) {
    //     auto str = readStr(root, name);
    //     return castEnum<Enum>(str);
    // }
    //
    // static void readShadersMap(yyjson_val *root, VoxHashMap<Render::ShaderStage, UUID> &out_shaders) {
    //     const auto obj = yyjson_obj_get(root, "shaders");
    //     VOX_CHECK(obj != nullptr, "Expected shaders property to be a json object");
    //
    //     yyjson_val *key, *val;
    //     yyjson_obj_iter iter = yyjson_obj_iter_with(obj);
    //
    //     while ((key = yyjson_obj_iter_next(&iter))) {
    //         val = yyjson_obj_iter_get_val(key);
    //         const auto stageStr = yyjson_get_str(key);
    //
    //         auto stage_enum = castEnum<Vox::Render::ShaderStage>(stageStr);
    //         auto shaderPath = yyjson_get_str(val);
    //         VOX_CHECK(shaderPath != nullptr, "Expected string for shader path");
    //         std::string_view val_str(shaderPath, yyjson_get_len(val));
    //         out_shaders[stage_enum] = Vox::InternedString(val_str);
    //     }
    // }

    Ref<Asset> MaterialImporter::load(FILE *data) {
        return *MaterialAsset::deserialize(data);

    // return new MaterialAsset(shaders, polygonMode, cullMode, topology, depthTest, depthWrite, depthCompareOp, texturePath);
    }
NS_END
