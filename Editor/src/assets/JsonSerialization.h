#pragma once

#include <yyjson.h>
#include <cstdio>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>
#include <type_traits>
#include <magic_enum/magic_enum.hpp>

#include <VoxCore/Define.h>
#include <VoxCore/math/UUID.h>
#include <VoxCore/containers/Containers.h>
#include <VoxCore/serialization/Serializable.h>
#include <VoxCore/math/UUID.h>

#include "../Define.h"

RESOURCES_NS
    class JsonSerialization : public SerializationType {
    public:
        ~JsonSerialization() override = default;
    };

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

        explicit SerializationContext(FILE *) : SerializationContext() {
        }

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

        void flush(FILE *file, bool pretty = true) const {
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

    template<>
    class Serializable<JsonSerialization> {
    public:
        virtual ~Serializable() = default;

        virtual void serialize(FILE *file) const = 0;

    protected:
        static SerializationContext<JsonSerialization> contextForReading(FILE *file) { return SerializationContext<JsonSerialization>::createForReading(file); }
        static SerializationContext<JsonSerialization> contextForWriting(FILE *) { return {}; }

        static inline bool readJsonUUID(yyjson_val *v, UUID &out) {
            if (!v) return false;
            if (yyjson_is_str(v)) {
                return parseUUIDString(std::string_view(yyjson_get_str(v), yyjson_get_len(v)), out);
            }
            if (yyjson_is_obj(v)) {
                auto hiVal = yyjson_obj_get(v, "hi");
                auto loVal = yyjson_obj_get(v, "lo");
                if (hiVal && loVal) {
                    out = UUID(yyjson_get_uint(hiVal), yyjson_get_uint(loVal));
                    return true;
                }
            }
            return false;
        }

        template<typename T>
        static inline yyjson_mut_val *makeJsonValue(yyjson_mut_doc *doc, const T &v) {
            if constexpr (std::is_same_v<T, bool>) {
                return yyjson_mut_bool(doc, v);
            } else if constexpr (std::is_integral_v<T>) {
                if constexpr (std::is_signed_v<T>) return yyjson_mut_sint(doc, static_cast<int64_t>(v));
                else return yyjson_mut_uint(doc, static_cast<uint64_t>(v));
            } else if constexpr (std::is_floating_point_v<T>) {
                return yyjson_mut_real(doc, static_cast<double>(v));
            } else if constexpr (std::is_same_v<T, std::string>) {
                return yyjson_mut_strncpy(doc, v.data(), v.size());
            } else if constexpr (std::is_enum_v<T>) {
                auto name = magic_enum::enum_name(v);
                if (!name.empty()) return yyjson_mut_strncpy(doc, name.data(), name.size());
                using U = std::underlying_type_t<T>;
                if constexpr (std::is_signed_v<U>) return yyjson_mut_sint(doc, static_cast<int64_t>(std::to_underlying(v)));
                else return yyjson_mut_uint(doc, static_cast<uint64_t>(std::to_underlying(v)));
            } else if constexpr (std::is_same_v<T, UUID>) {
                auto str = v.toString();
                return yyjson_mut_strncpy(doc, str.data(), str.size());
            } else {
                static_assert(!std::is_same_v<T, T>, "makeJsonValue: unsupported type");
            }
        }

        template<typename T>
        static inline bool readJsonValue(yyjson_val *val, T &out) {
            if constexpr (std::is_same_v<T, bool>) {
                if (!yyjson_is_bool(val)) return false;
                out = yyjson_get_bool(val);
            } else if constexpr (std::is_integral_v<T>) {
                if (!yyjson_is_int(val)) return false;
                if constexpr (std::is_signed_v<T>) out = static_cast<T>(yyjson_get_sint(val));
                else out = static_cast<T>(yyjson_get_uint(val));
            } else if constexpr (std::is_floating_point_v<T>) {
                if (!yyjson_is_num(val)) return false;
                out = static_cast<T>(yyjson_get_real(val));
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (!yyjson_is_str(val)) return false;
                out.assign(yyjson_get_str(val), yyjson_get_len(val));
            } else if constexpr (std::is_enum_v<T>) {
                if (yyjson_is_str(val)) {
                    auto e = magic_enum::enum_cast<T>(std::string_view(yyjson_get_str(val), yyjson_get_len(val)));
                    if (!e) return false;
                    out = *e;
                } else if (yyjson_is_int(val)) {
                    out = static_cast<T>(yyjson_get_sint(val));
                } else {
                    return false;
                }
            } else if constexpr (std::is_same_v<T, UUID>) {
                return readJsonUUID(val, out);
            } else {
                static_assert(!std::is_same_v<T, T>, "readJsonValue: неподдерживаемый тип");
            }
            return true;
        }

        template<typename K>
        static inline bool readJsonKey(yyjson_val *key, K &out) {
            std::string_view sv(yyjson_get_str(key), yyjson_get_len(key));
            if constexpr (std::is_same_v<K, std::string>) {
                out.assign(sv);
            } else if constexpr (std::is_enum_v<K>) {
                auto e = magic_enum::enum_cast<K>(sv);
                if (!e) return false;
                out = *e;
            } else if constexpr (std::is_integral_v<K> && !std::is_same_v<K, bool>) {
                auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), out);
                if (ec != std::errc{} || ptr != sv.data() + sv.size()) return false;
            }  else if constexpr (std::is_same_v<K, UUID>) {
                if (!parseUUIDString(sv, out)) return false;
            }else {
                static_assert(!std::is_same_v<K, K>, "readJsonKey: unsupported type");
            }
            return true;
        }

        template<typename K>
        static inline yyjson_mut_val *makeJsonKey(yyjson_mut_doc *doc, const K &k) {
            if constexpr (std::is_same_v<K, std::string>) {

                return yyjson_mut_strncpy(doc, k.data(), k.size());
            } else if constexpr (std::is_enum_v<K>) {
                auto name = magic_enum::enum_name(k);
                if (!name.empty()) return yyjson_mut_strncpy(doc, name.data(), name.size());
                char buf[24];
                auto res = std::to_chars(buf, buf + sizeof buf, std::to_underlying(k));
                return yyjson_mut_strncpy(doc, buf, static_cast<size_t>(res.ptr - buf));
            } else if constexpr (std::is_integral_v<K> && !std::is_same_v<K, bool>) {
                char buf[24];
                auto res = std::to_chars(buf, buf + sizeof buf, k);
                return yyjson_mut_strncpy(doc, buf, static_cast<size_t>(res.ptr - buf));
            }  else if constexpr (std::is_same_v<K, UUID>) {
                auto str = k.toString();
                return yyjson_mut_strncpy(doc, str.data(), str.size());
            } else {
                static_assert(!std::is_same_v<K, K>, "makeJsonKey: unsupported key");
            }
        }


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


        template<typename M>
            requires requires { typename M::key_type; typename M::mapped_type; }
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const M &map) {
            auto node = yyjson_mut_obj(doc);
            for (const auto &[k, v]: map) {
                yyjson_mut_obj_add(node, makeJsonKey(doc, k), makeJsonValue(doc, v));
            }
            yyjson_mut_obj_add_val(doc, obj, key, node);
        }

        template<typename T>
        static inline void serializeInternal(yyjson_mut_doc *doc, yyjson_mut_val *obj, const char *key, const std::vector<T> &vec) {
            auto arr = yyjson_mut_arr(doc);
            for (const auto &item : vec) {
                yyjson_mut_arr_append(arr, makeJsonValue(doc, item));
            }
            yyjson_mut_obj_add_val(doc, obj, key, arr);
        }

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
            for (char c: str) {
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
            return readJsonUUID(yyjson_obj_get(obj, key), out);
        }

        static inline bool parseHash128String(std::string_view str, Hash128 &out) {
            uint64_t hi = 0, lo = 0;
            int digitCount = 0;
            for (char c: str) {
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
                T item{};
                if (readJsonValue(val, item)) out.push_back(std::move(item));
            }
            return true;
        }

        template<typename M>
            requires requires { typename M::key_type; typename M::mapped_type; }
        static inline bool deserializeInternal(yyjson_val *obj, const char *key, M &out) {
            auto node = yyjson_obj_get(obj, key);
            if (!node || !yyjson_is_obj(node)) return false;

            out.clear();
            if constexpr (requires { out.reserve(size_t{}); }) {
                out.reserve(yyjson_obj_size(node));
            }

            size_t idx, max;
            yyjson_val *k, *v;
            yyjson_obj_foreach(node, idx, max, k, v) {
                typename M::key_type mk{};
                typename M::mapped_type mv{};
                if (readJsonKey(k, mk) && readJsonValue(v, mv)) {
                    out.insert_or_assign(std::move(mk), std::move(mv));
                }
            }
            return true;
        }
    };

NS_END
