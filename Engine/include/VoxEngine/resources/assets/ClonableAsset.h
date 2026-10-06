#pragma once
#include "Asset.h"
#include "VoxCore/Pointers.h"

RESOURCES_NS
    template<typename Derived, typename Base = Vox::Resources::Asset>
    class CloneableAsset : public Base {
    public:
        using Base::Base;

        UPtr<Derived> cloneTyped() const {
            return UPtr<Derived>(static_cast<Derived *>(this->cloneImpl(::operator new(sizeof(Derived)))));
        }

        UPtr<Base> clone() const override {
            return UPtr<Derived>(static_cast<Derived *>(this->cloneImpl(::operator new(sizeof(Derived)))));
        }

        void cloneTo(void* ptr) const override {
            static_cast<Derived *>(this->cloneImpl(ptr));
        }


    protected:
        virtual Derived *cloneImpl(void *ptr) const {
            return new (ptr) Derived(*static_cast<const Derived *>(this));
        }
    };
NS_END
