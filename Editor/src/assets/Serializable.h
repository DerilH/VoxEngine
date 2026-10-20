//
// Created by deril on 2/19/26.
//

#pragma once

#include <VoxCore/serialization/Serializable.h>
#include <VoxCore/serialization/SerializationContext.h>
#include "BlobView.h"
#include "serialization/JsonSerialization.h"
#include "../Define.h"

ASSETS_NS
    using SerializationType = Vox::Resources::Serialization::SerializationType;
    using BinarySerialization = Vox::Resources::Serialization::BinarySerialization;
    using Binary = Vox::Resources::Serialization::Binary;
    using binary = Vox::Resources::Serialization::binary;

    using JsonSerialization = Vox::Resources::Serialization::JsonSerialization;
    using Json = Vox::Resources::Serialization::Json;
    using json = Vox::Resources::Serialization::json;

    template<std::derived_from<SerializationType> T>
    using SerializationContext = Vox::Resources::Serialization::SerializationContext<T>;
    using BinaryContext = Vox::Resources::Serialization::BinaryContext;
    using JsonContext = Vox::Resources::Serialization::JsonContext;

    template<std::derived_from<SerializationType> T = BinarySerialization>
    using Serializable = Vox::Resources::Serialization::Serializable<T>;
NS_END
