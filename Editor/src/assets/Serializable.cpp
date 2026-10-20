//
// Created by deril on 2/20/26.
//

#include "Serializable.h"
#include <fstream>
#include <system_error>

SERIALIZATION_NS

    std::string Serializable<JsonSerialization>::toJsonString(bool pretty) const {
        yyjson_mut_doc *doc = yyjson_mut_doc_new(nullptr);
        if (!doc) {
            return {};
        }
        yyjson_mut_val *root = yyjson_mut_obj(doc);
        yyjson_mut_doc_set_root(doc, root);

        serialize(doc, root);

        size_t jsonSize = 0;
        yyjson_write_err writeErr;
        yyjson_write_flag flags = YYJSON_WRITE_ESCAPE_UNICODE;
        if (pretty) {
            flags |= YYJSON_WRITE_PRETTY;
        }
        char *jsonStr = yyjson_mut_write_opts(doc, flags, nullptr, &jsonSize, &writeErr);
        std::string result;
        if (jsonStr) {
            result.assign(jsonStr, jsonSize);
            free(jsonStr);
        }
        yyjson_mut_doc_free(doc);
        return result;
    }

    bool Serializable<JsonSerialization>::serializeToFile(const std::filesystem::path &path, bool pretty) const {
        std::string jsonStr = toJsonString(pretty);
        if (jsonStr.empty()) {
            return false;
        }
        if (path.has_parent_path()) {
            std::error_code ec;
            std::filesystem::create_directories(path.parent_path(), ec);
        }
        std::ofstream outFile(path, std::ios::out | std::ios::trunc | std::ios::binary);
        if (!outFile.is_open()) {
            return false;
        }
        outFile.write(jsonStr.data(), static_cast<std::streamsize>(jsonStr.size()));
        outFile.close();
        return true;
    }

    void Serializable<JsonSerialization>::serialize(FILE *file) const {
        serializeJson(file, true);
    }

    void Serializable<JsonSerialization>::serializeJson(FILE *file, bool pretty) const {
        if (!file) return;
        std::string jsonStr = toJsonString(pretty);
        if (!jsonStr.empty()) {
            fwrite(jsonStr.data(), sizeof(char), jsonStr.size(), file);
        }
    }

    bool Serializable<JsonSerialization>::deserializeFromString(std::string_view jsonStr) {
        if (jsonStr.empty()) {
            return false;
        }
        yyjson_read_err readErr;
        yyjson_doc *doc = yyjson_read_opts(const_cast<char *>(jsonStr.data()), jsonStr.size(), 0, nullptr, &readErr);
        if (!doc) {
            return false;
        }
        yyjson_val *root = yyjson_doc_get_root(doc);
        bool ok = deserialize(root);
        yyjson_doc_free(doc);
        return ok;
    }

    bool Serializable<JsonSerialization>::deserializeFromMemory(const void *data, size_t size) {
        if (!data || size == 0) {
            return false;
        }
        yyjson_read_err readErr;
        yyjson_doc *doc = yyjson_read_opts(static_cast<char *>(const_cast<void *>(data)), size, 0, nullptr, &readErr);
        if (!doc) {
            return false;
        }
        yyjson_val *root = yyjson_doc_get_root(doc);
        bool ok = deserialize(root);
        yyjson_doc_free(doc);
        return ok;
    }

    bool Serializable<JsonSerialization>::deserializeFromFile(const std::filesystem::path &path) {
        std::ifstream inFile(path, std::ios::in | std::ios::binary | std::ios::ate);
        if (!inFile.is_open()) {
            return false;
        }
        auto size = inFile.tellg();
        if (size <= 0) {
            return false;
        }
        std::string buffer(static_cast<size_t>(size), '\0');
        inFile.seekg(0, std::ios::beg);
        inFile.read(buffer.data(), size);
        inFile.close();
        return deserializeFromString(buffer);
    }

    bool Serializable<JsonSerialization>::deserialize(FILE *file) {
        return deserializeJson(file);
    }

    bool Serializable<JsonSerialization>::deserializeJson(FILE *file) {
        if (!file) return false;
        long curr = ftell(file);
        fseek(file, 0, SEEK_END);
        long size = ftell(file) - curr;
        fseek(file, curr, SEEK_SET);
        if (size <= 0) return false;
        std::string buffer(static_cast<size_t>(size), '\0');
        size_t readBytes = fread(buffer.data(), sizeof(char), static_cast<size_t>(size), file);
        buffer.resize(readBytes);
        return deserializeFromString(buffer);
    }

NS_END
