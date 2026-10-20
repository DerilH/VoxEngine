#pragma once
#include "../../Serializable.h"
#include "../../../Define.h"

ASSETS_NS
class ImportSettings : public Serializable<Binary> {
public:
    virtual ~ImportSettings() = default;
    void serialize(FILE *file) const override {}
};
NS_END
