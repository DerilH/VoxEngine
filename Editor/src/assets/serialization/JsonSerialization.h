#pragma once

#include <yyjson.h>
#include <cstdio>
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
#include <VoxCore/serialization/SerializationContext.h>

SERIALIZATION_NS

    class JsonSerialization : public SerializationType {
    public:
        ~JsonSerialization() override = default;
    };

    using Json = JsonSerialization;
    using json = JsonSerialization;

    template<>
    class SerializationContext<JsonSerialization> {
    public:
        yyjson_mut_doc *doc = nullptr;
        yyjson_mut_val *root = nullptr;
        yyjson_doc *readDoc = nullptr;
        yyjson_val *readRoot = nullptr;
        bool ownsDoc = true;
        bool ownsReadDoc = true;

        SerializationContext() {
            doc = yyjson_mut_doc_new(nullptr);
            root = yyjson_mut_obj(doc);
            yyjson_mut_doc_set_root(doc, root);
        }

        explicit SerializationContext(FILE *) : SerializationContext() {}

        SerializationContext(yyjson_mut_doc *doc, yyjson_mut_val *root)
            : doc(doc), root(root), ownsDoc(false) {
        }

        ~SerializationContext() {
            reset();
        }

        void reset() {
            if (ownsDoc && doc) {
                yyjson_mut_doc_free(doc);
                doc = nullptr;
                root = nullptr;
            }
            if (ownsReadDoc && readDoc) {
                yyjson_doc_free(readDoc);
                readDoc = nullptr;
                readRoot = nullptr;
            }
        }

        SerializationContext(const SerializationContext &) = delete;
        SerializationContext &operator=(const SerializationContext &) = delete;

        SerializationContext(SerializationContext &&other) noexcept
            : doc(other.doc), root(other.root), readDoc(other.readDoc), readRoot(other.readRoot),
              ownsDoc(other.ownsDoc), ownsReadDoc(other.ownsReadDoc) {
            other.doc = nullptr;
            other.root = nullptr;
            other.readDoc = nullptr;
            other.readRoot = nullptr;
            other.ownsDoc = false;
            other.ownsReadDoc = false;
        }

        SerializationContext &operator=(SerializationContext &&other) noexcept {
            if (this != &other) {
                reset();
                doc = other.doc;
                root = other.root;
                readDoc = other.readDoc;
                readRoot = other.readRoot;
                ownsDoc = other.ownsDoc;
                ownsReadDoc = other.ownsReadDoc;
                other.doc = nullptr;
                other.root = nullptr;
                other.readDoc = nullptr;
                other.readRoot = nullptr;
                other.ownsDoc = false;
                other.ownsReadDoc = false;
            }
            return *this;
        }

        void flush(FILE *file, bool pretty = true) {
            if (!file || !doc) return;
            size_t jsonSize = 0;
            yyjson_write_err writeErr;
            yyjson_write_flag flags = YYJSON_WRITE_ESCAPE_UNICODE;
            if (pretty) flags |= YYJSON_WRITE_PRETTY;
            char *jsonStr = yyjson_mut_write_opts(doc, flags, nullptr, &jsonSize, &writeErr);
            if (jsonStr) {
                fwrite(jsonStr, sizeof(char), jsonSize, file);
                free(jsonStr);
            }
        }

        std::string toString(bool pretty = true) const {
            if (!doc) return {};
            size_t jsonSize = 0;
            yyjson_write_err writeErr;
            yyjson_write_flag flags = YYJSON_WRITE_ESCAPE_UNICODE;
            if (pretty) flags |= YYJSON_WRITE_PRETTY;
            char *jsonStr = yyjson_mut_write_opts(doc, flags, nullptr, &jsonSize, &writeErr);
            std::string result;
            if (jsonStr) {
                result.assign(jsonStr, jsonSize);
                free(jsonStr);
            }
            return result;
        }

        static SerializationContext createForReading(std::string_view jsonStr) {
            SerializationContext ctx;
            ctx.reset();
            yyjson_read_err readErr;
            ctx.readDoc = yyjson_read_opts(const_cast<char *>(jsonStr.data()), jsonStr.size(), 0, nullptr, &readErr);
            if (ctx.readDoc) {
                ctx.readRoot = yyjson_doc_get_root(ctx.readDoc);
            }
            ctx.ownsReadDoc = true;
            return ctx;
        }

        static SerializationContext createForReading(FILE *file) {
            if (!file) return {};
            long curr = ftell(file);
            fseek(file, 0, SEEK_END);
            long size = ftell(file) - curr;
            fseek(file, curr, SEEK_SET);
            if (size <= 0) return {};
            std::string buffer(static_cast<size_t>(size), '\0');
            size_t readBytes = fread(buffer.data(), sizeof(char), static_cast<size_t>(size), file);
            buffer.resize(readBytes);
            return createForReading(buffer);
        }
    };

    using JsonContext = SerializationContext<JsonSerialization>;
    using json_context = SerializationContext<JsonSerialization>;

    template<>
    class Serializable<JsonSerialization> {
    public:
        virtual ~Serializable() = default;

        // Unified virtual method in base Serializable
        virtual void serialize(FILE *file) const = 0;

        // Convenience methods
        std::string toJsonString(bool pretty = true) const;
        bool serializeToFile(const std::filesystem::path &path, bool pretty = true) const;

        // --- UNIFIED SERIALIZATION (with context and key) ---
        template<typename T>
        static inline void serializeInternal(FILE *, SerializationContext<JsonSerialization> &ctx, const char *key, const T &val) {
            serializeInternal(ctx.doc, ctx.root, key, val);
        }

        template<typename T, typename... Rest>
        requires (sizeof...(Rest) > 0 && sizeof...(Rest) % 2 == 0)
        static inline void serializeInternal(FILE *file, SerializationContext<JsonSerialization> &ctx, const char *key, const T &val, Rest &&... rest) {
            serializeInternal(file, ctx, key, val);
            serializeInternal(file, ctx, std::forward<Rest>(rest)...);
        }

        // --- UNIFIED DESERIALIZATION (with context and key) ---
        template<typename T>
        static inline bool deserializeInternal(FILE *, SerializationContext<JsonSerialization> &ctx, const char *key, T &val) {
            if (!ctx.readRoot) return false;
            return deserializeInternal(ctx.readRoot, key, val);
        }

        template<typename T, typename... Rest>
        requires (sizeof...(Rest) > 0 && sizeof...(Rest) % 2 == 0)
        static inline bool deserializeInternal(FILE *file, SerializationContext<JsonSerialization> &ctx, const char *key, T &val, Rest &&... rest) {
            bool ok = deserializeInternal(file, ctx, key, val);
            return ok && deserializeInternal(file, ctx, std::forward<Rest>(rest)...);
        }

        // --- DIRECT JSON SERIALIZATION HELPERS ---

        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, bool val) {
            yyjson_mut_obj_add_bool(doc, obj, key, val);
        }

        template<typename T>
        requires std::is_integral_v<T> && (!std::is_same_v<T, bool>)
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, T val) {
            if constexpr (std::is_signed_v<T>) {
                yyjson_mut_obj_add_sint(doc, obj, key, static_cast<int64_t>(val));
            } else {
                yyjson_mut_obj_add_uint(doc, obj, key, static_cast<uint64_t>(val));
            }
        }

        template<typename T>
        requires std::is_floating_point_v<T>
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, T val) {
            yyjson_mut_obj_add_real(doc, obj, key, static_cast<double>(val));
        }

        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const std::string &val) {
            yyjson_mut_obj_add_strncpy(doc, obj, key, val.data(), val.size());
        }

        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, std::string_view val) {
            yyjson_mut_obj_add_strncpy(doc, obj, key, val.data(), val.size());
        }

        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const char *val) {
            if (val) {
                yyjson_mut_obj_add_strcpy(doc, obj, key, val);
            } else {
                yyjson_mut_obj_add_null(doc, obj, key);
            }
        }

        template<typename T>
        requires std::is_enum_v<T>
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const T &val) {
            auto name = magic_enum::enum_name(val);
            if (!name.empty()) {
                yyjson_mut_obj_add_strncpy(doc, obj, key, name.data(), name.size());
            } else {
                yyjson_mut_obj_add_int(doc, obj, key, std::to_underlying(val));
            }
        }

        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const UUID &uuid) {
            auto str = uuid.toString();
            yyjson_mut_obj_add_strncpy(doc, obj, key, str.data(), str.size());
        }

        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const Hash128 &hash) {
            auto hObj = yyjson_mut_obj(doc);
            yyjson_mut_obj_add_uint(doc, hObj, "hi", hash.hi);
            yyjson_mut_obj_add_uint(doc, hObj, "lo", hash.lo);
            yyjson_mut_obj_add_val(doc, obj, key, hObj);
        }

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
                    yyjson_mut_arr_add_strncpy(doc, arr, item.data(), item.size());
                } else if constexpr (std::is_enum_v<T>) {
                    auto name = magic_enum::enum_name(item);
                    if (!name.empty()) {
                        yyjson_mut_arr_add_strncpy(doc, arr, name.data(), name.size());
                    } else {
                        yyjson_mut_arr_add_int(doc, arr, std::to_underlying(item));
                    }
                }
            }
            yyjson_mut_obj_add_val(doc, obj, key, arr);
        }

        // --- DIRECT JSON DESERIALIZATION HELPERS ---

        static inline bool deserializeInternal(yyjson_val *obj, const char *key, bool &out) {
            auto v = yyjson_obj_get(obj, key);
            if (v && yyjson_is_bool(v)) {
                out = yyjson_get_bool(v);
                return true;
            }
            return false;
        }

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

        static inline bool deserializeInternal(yyjson_val *obj, const char *key, std::string &out) {
            auto v = yyjson_obj_get(obj, key);
            if (v && yyjson_is_str(v)) {
                out.assign(yyjson_get_str(v), yyjson_get_len(v));
                return true;
            }
            return false;
        }

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
    using JsonContext = Vox::Resources::Serialization::JsonContext;
    using json_context = Vox::Resources::Serialization::json_context;
NS_END
