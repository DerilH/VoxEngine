//
// Created by deril on 2/20/26.
//

#pragma once

#include "ostream"
#include "BlobView.h"
#include <cstdio>
#include "tuple"

SERIALIZATION_NS

#define SERIALIZE(...) \
    void serialize(FILE* file) { \
      serializeInternal(file, __VA_ARGS__); \
    }
#define DESERIALIZE(outClass, ...) \
    static Serializable *deserialize(FILE *file, Serializable *out = nullptr) {\
        if (out == nullptr) out = outClas();\
        baseClass::deserialize(file, out);\
        deserializeInternal(file, __VA_ARGS__);\
    }\

#define SERIALIZE_CHAINED(base, ...) \
    void serialize(FILE* file) { \
      base::serialize(file);                   \
      serializeInternal(file, __VA_ARGS__); \
    }

#define BLOB(...) Vox::Resources::Serialization::BlobView(__VA_ARGS__)

template<typename T>
    concept Deserializable = requires(FILE* f) {
        { T::deserialize(f) } -> std::same_as<T*>;
    };
    class Serializable {
    protected:
        //DESERIALIZATION
        template<typename... Args>
        static inline void deserializeInternal(FILE *file, Args &&... args) {
            (deserializeInternal(file, args), ...);
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        static inline void deserializeInternal(FILE *file, const T &arithmetic) {
            fread(static_cast<void *>(&arithmetic), sizeof(T), 1, file);
        }

        template<typename T>
        requires std::is_base_of_v<BlobView, T>
        static inline void deserializeInternal(FILE *file, const T &blob) {

            fread(static_cast<void *>(&blob.elementSize), sizeof(blob.elementSize), 1, file);
            fread(static_cast<void *>(&blob.count), sizeof(blob.count), 1, file);
            fwrite(blob.pData, blob.elementSize, blob.count, file);
        }

        //SERIALIZATION
        template<typename... Args>
        static inline void serializeInternal(FILE *file, Args &&... args) {
            (serializeInternal(file, args), ...);
        }

        template<typename T>
        requires std::is_arithmetic_v<T>
        static inline void serializeInternal(FILE *file, const T &arithmetic) {
            fwrite(static_cast<const void *>(&arithmetic), sizeof(T), 1, file);
        }

        template<typename T>
        requires std::is_base_of_v<BlobView, T>
        static inline void serializeInternal(FILE *file, const T &blob) {
            fwrite(static_cast<const void *>(&blob.elementSize), sizeof(blob.elementSize), 1, file);
            fwrite(static_cast<const void *>(&blob.count), sizeof(blob.count), 1, file);
            fwrite(blob.pData, blob.elementSize, blob.count, file);
        }

    public:
        virtual void serialize(FILE *file) = 0;
    };
NS_END