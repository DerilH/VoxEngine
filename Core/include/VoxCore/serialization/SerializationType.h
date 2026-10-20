#pragma once

#include <VoxCore/Define.h>

SERIALIZATION_NS

    class SerializationType {
    public:
        virtual ~SerializationType() = 0;
    };
    inline SerializationType::~SerializationType() = default;

    class BinarySerialization : public SerializationType {
    public:
        ~BinarySerialization() override = default;
    };

    using Binary = BinarySerialization;
    using binary = BinarySerialization;

NS_END

VOX_NS
    using SerializationType = Vox::Resources::Serialization::SerializationType;
    using BinarySerialization = Vox::Resources::Serialization::BinarySerialization;
    using Binary = Vox::Resources::Serialization::Binary;
    using binary = Vox::Resources::Serialization::binary;
NS_END
