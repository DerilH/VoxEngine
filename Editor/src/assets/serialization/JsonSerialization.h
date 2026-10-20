#pragma once

#include <yyjson.h>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <concepts>
#include <type_traits>
#include <magic_enum/magic_enum.hpp>

#include <VoxCore/Define.h>
#include <VoxCore/math/UUID.h>
#include <VoxCore/math/Hash128.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/serialization/Serializable.h>

SERIALIZATION_NS

    class JsonSerialization : public SerializationType {
    public:
        ~JsonSerialization() override = default;
    };

    using Json = JsonSerialization;
    using json = JsonSerialization;

    template<>
    class Serializable<JsonSerialization> {
    public:
        virtual ~Serializable() = default;

        // Core pure virtual method for JSON serialization
        virtual void serialize(yyjson_mut_doc *doc, yyjson_mut_val *root) const = 0;

        // Virtual method for JSON deserialization
        virtual bool deserialize(yyjson_val *root) { return true; }

        // Convenience serialization methods
        std::string toJsonString(bool pretty = true) const;
        bool serializeToFile(const std::filesystem::path &path, bool pretty = true) const;
        void serialize(FILE *file) const;
        void serializeJson(FILE *file, bool pretty = true) const;

        // Convenience deserialization methods
        bool deserializeFromString(std::string_view jsonStr);
        bool deserializeFromFile(const std::filesystem::path &path);
        bool deserializeFromMemory(const void *data, size_t size);
        bool deserialize(FILE *file);
        bool deserializeJson(FILE *file);

        // --- SERIALIZATION HELPERS ---

        // Booleans
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, bool val) {
            yyjson_mut_obj_add_bool(doc, obj, key, val);
        }

        // Integral types
        template<typename T>
        requires std::is_integral_v<T> && (!std::is_same_v<T, bool>)
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, T val) {
            if constexpr (std::is_signed_v<T>) {
                yyjson_mut_obj_add_sint(doc, obj, key, static_cast<int64_t>(val));
            } else {
                yyjson_mut_obj_add_uint(doc, obj, key, static_cast<uint64_t>(val));
            }
        }

        // Floating point
        template<typename T>
        requires std::is_floating_point_v<T>
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, T val) {
            yyjson_mut_obj_add_real(doc, obj, key, static_cast<double>(val));
        }

        // Strings
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const std::string &val) {
            yyjson_mut_obj_add_strn(doc, obj, key, val.data(), val.size());
        }

        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, std::string_view val) {
            yyjson_mut_obj_add_strn(doc, obj, key, val.data(), val.size());
        }

        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const char *val) {
            if (val) {
                yyjson_mut_obj_add_str(doc, obj, key, val);
            } else {
                yyjson_mut_obj_add_null(doc, obj, key);
            }
        }

        // Enums
        template<typename T>
        requires std::is_enum_v<T>
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const T &val) {
            auto name = magic_enum::enum_name(val);
            if (!name.empty()) {
                yyjson_mut_obj_add_strn(doc, obj, key, name.data(), name.size());
            } else {
                yyjson_mut_obj_add_int(doc, obj, key, std::to_underlying(val));
            }
        }

        // UUID
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const UUID &uuid) {
            auto str = uuid.toString();
            yyjson_mut_obj_add_strn(doc, obj, key, str.data(), str.size());
        }

        // Hash128
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const Hash128 &hash) {
            auto hObj = yyjson_mut_obj(doc);
            yyjson_mut_obj_add_uint(doc, hObj, "hi", hash.hi);
            yyjson_mut_obj_add_uint(doc, hObj, "lo", hash.lo);
            yyjson_mut_obj_add_val(doc, obj, key, hObj);
        }

        // Nested Serializable<JsonSerialization>
        template<typename Sub>
        requires std::is_base_of_v<Serializable<JsonSerialization>, Sub>
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const Sub &sub) {
            auto child = yyjson_mut_obj(doc);
            sub.serialize(doc, child);
            yyjson_mut_obj_add_val(doc, obj, key, child);
        }

        // Vector of elements
        template<typename T>
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const std::vector<T> &vec) {
            auto arr = yyjson_mut_arr(doc);
            for (const auto &item : vec) {
                if constexpr (std::is_same_v<T, bool>) {
                    yyjson_mut_arr_add_bool(doc, arr, item);
                } else if constexpr (std::is_integral_v<T>) {
                    if constexpr (std::is_signed_v<T>) {
                        yyjson_mut_arr_add_sint(doc, arr, static_cast<int64_t>(item));
                    } else {
                        yyjson_mut_arr_add_uint(doc, arr, static_cast<uint64_t>(item));
                    }
                } else if constexpr (std::is_floating_point_v<T>) {
                    yyjson_mut_arr_add_real(doc, arr, static_cast<double>(item));
                } else if constexpr (std::is_same_v<T, std::string>) {
                    yyjson_mut_arr_add_strn(doc, arr, item.data(), item.size());
                } else if constexpr (std::is_enum_v<T>) {
                    auto name = magic_enum::enum_name(item);
                    if (!name.empty()) {
                        yyjson_mut_arr_add_strn(doc, arr, name.data(), name.size());
                    } else {
                        yyjson_mut_arr_add_int(doc, arr, std::to_underlying(item));
                    }
                } else if constexpr (std::is_base_of_v<Serializable<JsonSerialization>, T>) {
                    auto childObj = yyjson_mut_obj(doc);
                    item.serialize(doc, childObj);
                    yyjson_mut_arr_add_val(arr, childObj);
                }
            }
            yyjson_mut_obj_add_val(doc, obj, key, arr);
        }

        // --- DESERIALIZATION HELPERS ---

        // Booleans
        static inline bool deserializeInternal(yyjson_val *obj, const char *key, bool &out) {
            auto v = yyjson_obj_get(obj, key);
            if (v && yyjson_is_bool(v)) {
                out = yyjson_get_bool(v);
                return true;
            }
            return false;
        }

        // Integral types
        template<typename T>
        requires std::is_integral_v<T> && (!std::is_same_v<T, bool>)
        static inline bool deserializeInternal(yyjson_val *obj, const char *key, T &out) {
            auto v = yyjson_obj_get(obj, key);
            if (v && yyjson_is_num(v)) {
                if constexpr (std::is_signed_v<T>) {
                    out = static_cast<T>(yyjson_get_sint(v));
                } else {
                    out = static_cast<T>(yyjson_get_uint(v));
                }
                return true;
            }
            return false;
        }

        // Floating point
        template<typename T>
        requires std::is_floating_point_v<T>
        static inline bool deserializeInternal(yyjson_val *obj, const char *key, T &out) {
            auto v = yyjson_obj_get(obj, key);
            if (v && yyjson_is_num(v)) {
                out = static_cast<T>(yyjson_get_real(v));
                return true;
            }
            return false;
        }

        // Strings
        static inline bool deserializeInternal(yyjson_val *obj, const char *key, std::string &out) {
            auto v = yyjson_obj_get(obj, key);
            if (v && yyjson_is_str(v)) {
                out.assign(yyjson_get_str(v), yyjson_get_len(v));
                return true;
            }
            return false;
        }

        // Enums
        template<typename T>
        requires std::is_enum_v<T>
        static inline bool deserializeInternal(yyjson_val *obj, const char *key, T &out) {
            auto v = yyjson_obj_get(obj, key);
            if (!v) return false;
            if (yyjson_is_str(v)) {
                auto valOpt = magic_enum::enum_cast<T>(std::string_view(yyjson_get_str(v), yyjson_get_len(v)));
                if (valOpt) {
                    out = *valOpt;
                    return true;
                }
            } else if (yyjson_is_num(v)) {
                out = static_cast<T>(yyjson_get_sint(v));
                return true;
            }
            return false;
        }

        // UUID
        static inline bool parseUUIDString(std::string_view str, UUID &out) {
            uint64_t hi = 0, lo = 0;
            int digitCount = 0;
            for (char c : str) {
                if (c == '-') continue;
                int hex = -1;
                if (c >= '0' && c <= '9') hex = c - '0';
                else if (c >= 'a' && c <= 'f') hex = c - 'a' + 10;
                else if (c >= 'A' && c <= 'F') hex = c - 'A' + 10;
                else return false;

                if (digitCount < 16) {
                    hi = (hi << 4) | static_cast<uint64_t>(hex);
                } else if (digitCount < 32) {
                    lo = (lo << 4) | static_cast<uint64_t>(hex);
                } else {
                    return false;
                }
                digitCount++;
            }
            if (digitCount != 32) return false;
            out = UUID(hi, lo);
            return true;
        }

        static inline bool deserializeInternal(yyjson_val *obj, const char *key, UUID &out) {
            auto v = yyjson_obj_get(obj, key);
            if (!v) return false;
            if (yyjson_is_str(v)) {
                return parseUUIDString(std::string_view(yyjson_get_str(v), yyjson_get_len(v)), out);
            } else if (yyjson_is_obj(v)) {
                auto hiVal = yyjson_obj_get(v, "hi");
                auto loVal = yyjson_obj_get(v, "lo");
                if (hiVal && loVal) {
                    out = UUID(yyjson_get_uint(hiVal), yyjson_get_uint(loVal));
                    return true;
                }
            }
            return false;
        }

        // Hash128
        static inline bool parseHash128String(std::string_view str, Hash128 &out) {
            uint64_t hi = 0, lo = 0;
            int digitCount = 0;
            for (char c : str) {
                int hex = -1;
                if (c >= '0' && c <= '9') hex = c - '0';
                else if (c >= 'a' && c <= 'f') hex = c - 'a' + 10;
                else if (c >= 'A' && c <= 'F') hex = c - 'A' + 10;
                else return false;

                if (digitCount < 16) {
                    hi = (hi << 4) | static_cast<uint64_t>(hex);
                } else if (digitCount < 32) {
                    lo = (lo << 4) | static_cast<uint64_t>(hex);
                } else {
                    return false;
                }
                digitCount++;
            }
            if (digitCount != 32) return false;
            out.hi = hi;
            out.lo = lo;
            return true;
        }

        static inline bool deserializeInternal(yyjson_val *obj, const char *key, Hash128 &out) {
            auto v = yyjson_obj_get(obj, key);
            if (!v) return false;
            if (yyjson_is_obj(v)) {
                auto hiVal = yyjson_obj_get(v, "hi");
                auto loVal = yyjson_obj_get(v, "lo");
                if (hiVal && loVal) {
                    out.hi = yyjson_get_uint(hiVal);
                    out.lo = yyjson_get_uint(loVal);
                    return true;
                }
            } else if (yyjson_is_str(v)) {
                return parseHash128String(std::string_view(yyjson_get_str(v), yyjson_get_len(v)), out);
            }
            return false;
        }

        // Nested Serializable<JsonSerialization>
        template<typename Sub>
        requires std::is_base_of_v<Serializable<JsonSerialization>, Sub>
        static inline bool deserializeInternal(yyjson_val *obj, const char *key, Sub &out) {
            auto v = yyjson_obj_get(obj, key);
            if (!v || !yyjson_is_obj(v)) return false;
            return out.deserialize(v);
        }

        // Vector
        template<typename T>
        static inline bool deserializeInternal(yyjson_val *obj, const char *key, std::vector<T> &out) {
            auto arr = yyjson_obj_get(obj, key);
            if (!arr || !yyjson_is_arr(arr)) return false;
            out.clear();
            out.reserve(yyjson_arr_size(arr));
            size_t idx, max;
            yyjson_val *val;
            yyjson_arr_foreach(arr, idx, max, val) {
                if constexpr (std::is_same_v<T, bool>) {
                    if (yyjson_is_bool(val)) out.push_back(yyjson_get_bool(val));
                } else if constexpr (std::is_integral_v<T>) {
                    if (yyjson_is_num(val)) {
                        if constexpr (std::is_signed_v<T>) {
                            out.push_back(static_cast<T>(yyjson_get_sint(val)));
                        } else {
                            out.push_back(static_cast<T>(yyjson_get_uint(val)));
                        }
                    }
                } else if constexpr (std::is_floating_point_v<T>) {
                    if (yyjson_is_num(val)) out.push_back(static_cast<T>(yyjson_get_real(val)));
                } else if constexpr (std::is_same_v<T, std::string>) {
                    if (yyjson_is_str(val)) out.emplace_back(yyjson_get_str(val), yyjson_get_len(val));
                } else if constexpr (std::is_enum_v<T>) {
                    if (yyjson_is_str(val)) {
                        auto e = magic_enum::enum_cast<T>(std::string_view(yyjson_get_str(val), yyjson_get_len(val)));
                        if (e) out.push_back(*e);
                    } else if (yyjson_is_num(val)) {
                        out.push_back(static_cast<T>(yyjson_get_sint(val)));
                    }
                } else if constexpr (std::is_base_of_v<Serializable<JsonSerialization>, T>) {
                    if (yyjson_is_obj(val)) {
                        T item{};
                        if (item.deserialize(val)) out.push_back(std::move(item));
                    }
                }
            }
            return true;
        }
    };

NS_END

VOX_NS
    using JsonSerialization = Vox::Resources::Serialization::JsonSerialization;
    using Json = Vox::Resources::Serialization::Json;
    using json = Vox::Resources::Serialization::json;
NS_END
