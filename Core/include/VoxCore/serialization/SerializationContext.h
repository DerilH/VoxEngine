#pragma once

#include <cstdio>
#include <concepts>
#include "SerializationType.h"

RESOURCES_NS
    template<std::derived_from<SerializationType> T>
    class SerializationContext;

    template<>
    class SerializationContext<BinarySerialization> {
    public:
        SerializationContext() = default;
        explicit SerializationContext(FILE *) {}

        void flush(FILE * = nullptr) const {}
    };
NS_END