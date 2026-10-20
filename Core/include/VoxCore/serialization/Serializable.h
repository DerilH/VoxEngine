#pragma once

#include <cstdio>
#include <utility>
#include <type_traits>
#include <concepts>
#include <string>

#include <VoxCore/Define.h>
#include <VoxCore/math/UUID.h>
#include <VoxCore/math/Hash128.h>
#include <VoxCore/containers/Containers.h>
#include "SerializationType.h"
#include "BlobView.h"

SERIALIZATION_NS

    template<std::derived_from<SerializationType> T = BinarySerialization>
    class Serializable;

    template<>
    class Serializable<BinarySerialization> {
    public:
        // DESERIALIZATION
        template<typename... Args>
        static inline void deserializeInternal(FILE *file, Args &... args) {
            (deserializeInternal(file, args), ...);
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        static inline void deserializeInternal(FILE *file, T &arithmetic) {
            fread(static_cast<void *>(&arithmetic), sizeof(T), 1, file);
        }

        template<typename T>
        requires std::is_enum_v<T>
        static inline void deserializeInternal(FILE *file, T &val) {
            std::underlying_type_t<T> underlying{};
            fread(&underlying, sizeof(underlying), 1, file);
            val = static_cast<T>(underlying);
        }

        static inline void deserializeInternal(FILE *file, UUID &uuid) {
            uint64_t hi = 0, lo = 0;
            fread(&hi, sizeof(hi), 1, file);
            fread(&lo, sizeof(lo), 1, file);
            uuid = UUID(hi, lo);
        }

        static inline void deserializeInternal(FILE *file, Hash128 &hash) {
            fread(&hash.hi, sizeof(hash.hi), 1, file);
            fread(&hash.lo, sizeof(hash.lo), 1, file);
        }

        static inline void deserializeInternal(FILE *file, std::string &str) {
            uint32_t len = 0;
            fread(&len, sizeof(len), 1, file);
            str.resize(len);
            if (len > 0) {
                fread(str.data(), sizeof(char), len, file);
            }
        }

        static inline void deserializeInternal(FILE *file, void *dest, size_t size, size_t count = 1) {
            if (dest && size > 0 && count > 0) {
                fread(dest, size, count, file);
            }
        }

        // SERIALIZATION
        template<typename... Args>
        static inline void serializeInternal(FILE *file, const Args &... args) {
            (serializeInternal(file, args), ...);
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        static inline void serializeInternal(FILE *file, const T &arithmetic) {
            fwrite(static_cast<const void *>(&arithmetic), sizeof(T), 1, file);
        }

        template<typename T>
        requires std::is_enum_v<T>
        static inline void serializeInternal(FILE *file, const T &val) {
            auto underlying = std::to_underlying(val);
            fwrite(&underlying, sizeof(underlying), 1, file);
        }

        static inline void serializeInternal(FILE *file, const UUID &uuid) {
            auto hi = uuid.getHi();
            auto lo = uuid.getLo();
            fwrite(&hi, sizeof(hi), 1, file);
            fwrite(&lo, sizeof(lo), 1, file);
        }

        static inline void serializeInternal(FILE *file, const Hash128 &hash) {
            fwrite(&hash.hi, sizeof(hash.hi), 1, file);
            fwrite(&hash.lo, sizeof(hash.lo), 1, file);
        }

        template<typename T>
        requires std::is_base_of_v<BlobView, T>
        static inline void serializeInternal(FILE *file, const T &blob) {
            fwrite(static_cast<const void *>(&blob.elementSize), sizeof(blob.elementSize), 1, file);
            fwrite(static_cast<const void *>(&blob.count), sizeof(blob.count), 1, file);
            if (blob.pData && blob.count > 0) {
                fwrite(blob.pData, blob.elementSize, blob.count, file);
            }
        }

        static inline void serializeInternal(FILE *file, const std::string &str) {
            uint32_t len = static_cast<uint32_t>(str.size());
            fwrite(&len, sizeof(len), 1, file);
            if (len > 0) {
                fwrite(str.data(), sizeof(char), len, file);
            }
        }

    public:
        virtual ~Serializable() = default;

        virtual void serialize(FILE *file) const = 0;
    };

NS_END

VOX_NS
    template<std::derived_from<SerializationType> T = BinarySerialization>
    using Serializable = Vox::Resources::Serialization::Serializable<T>;
NS_END
