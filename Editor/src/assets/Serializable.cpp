//
// Created by deril on 2/20/26.
//

#include "Serializable.h"
#include <system_error>

SERIALIZATION_NS

    std::string Serializable<JsonSerialization>::toJsonString(bool pretty) const {
        FILE *f = tmpfile();
        if (!f) return {};
        serialize(f);
        long size = ftell(f);
        if (size <= 0) {
            fclose(f);
            return {};
        }
        rewind(f);
        std::string result(static_cast<size_t>(size), '\0');
        fread(result.data(), sizeof(char), static_cast<size_t>(size), f);
        fclose(f);
        return result;
    }

    bool Serializable<JsonSerialization>::serializeToFile(const std::filesystem::path &path, bool pretty) const {
        if (path.has_parent_path()) {
            std::error_code ec;
            std::filesystem::create_directories(path.parent_path(), ec);
        }
        FILE *f = fopen(path.c_str(), "wb");
        if (!f) return false;
        serialize(f);
        fclose(f);
        return true;
    }

    // Explicit instantiations or empty namespace
NS_END
