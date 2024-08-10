#ifndef WIND_TYPE_BOXED_H
#define WIND_TYPE_BOXED_H

#include "itype.h"

namespace wind {

class BoxedType : public Type {
public:
    std::shared_ptr<Type>* ty;

    BoxedType() {
        ty = new std::shared_ptr<Type>(wind::Type::VOID);
    }

    ~BoxedType() {
        delete ty;
    }

    bool isVoid() override {
        return get()->isVoid();
    }

    bool isBool() override {
        return get()->isBool();
    }

    bool isVoidPtr() override {
        return get()->isVoidPtr();
    }

    bool isPtr() override {
        return get()->isPtr();
    }

    bool isSigned() override {
        return get()->isSigned();
    }

    bool isUnsigned() override {
        return get()->isUnsigned();
    }

    bool isInteger() override {
        return get()->isInteger();
    }

    bool isFloat() override {
        return get()->isFloat();
    }

    bool isStruct() override {
        return get()->isStruct();
    }

    bool isProtocol() override {
        return get()->isProtocol();
    }

    void set(std::shared_ptr<Type> ty) {
        *this->ty = ty;
    }

    std::shared_ptr<Type> get() override {
        return *ty;
    }

    std::string toString() override {
        return get()->toString();
    }

    std::string hash() override {
        return get()->hash();
    }
};
}


#endif