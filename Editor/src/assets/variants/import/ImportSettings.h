#pragma once
#include "../../JsonSerialization.h"
#include "../../../Define.h"

ASSETS_NS
class ImportSettings : public Resources::Serializable<Resources::JsonSerialization> {
public:
    ~ImportSettings() override = default;
    virtual ImportSettings* clone() = 0;


    void serialize(FILE *file) const override {
    }
};
NS_END
