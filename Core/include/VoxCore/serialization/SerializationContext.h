#pragma once

#include <cstdio>
#include <concepts>
#include <VoxCore/Define.h>
#include "SerializationType.h"

SERIALIZATION_NS

    template<std::derived_from<SerializationType> T>
    class SerializationContext;

    template<>
    class SerializationContext<BinarySerialization> {
    public:
        SerializationContext() = default;
        explicit SerializationContext(FILE *) {}

        void flush(FILE * = nullptr) const {}
    };

    using BinaryContext = SerializationContext<BinarySerialization>;
    using binary_context = SerializationContext<BinarySerialization>;

NS_END

VOX_NS
    template<std::derived_from<SerializationType> T>
    using SerializationContext = Vox::Resources::Serialization::SerializationContext<T>;
    using BinaryContext = Vox::Resources::Serialization::BinaryContext;
    using binary_context = Vox::Resources::Serialization::binary_context;
NS_END
