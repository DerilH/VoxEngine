#pragma once

#include <VoxCore/Define.h>

RESOURCES_NS
    class SerializationType {
    public:
        virtual ~SerializationType() = 0;
    };
    inline SerializationType::~SerializationType() = default;

    class BinarySerialization : public SerializationType {
    public:
        ~BinarySerialization() override = default;
    };
NS_END
