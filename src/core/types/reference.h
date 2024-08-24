#ifndef WIND_TYPE_QUOTE_H
#define WIND_TYPE_QUOTE_H

#include "itype.h"

namespace wind {

class ReferenceType : public Type {
public:
    std::shared_ptr<Type> basic;

    ReferenceType(std::shared_ptr<Type> basic) : basic(basic) {}

    std::string getName() override {
        return basic->getName();
    }

    std::vector<std::shared_ptr<Type>> getParameters() override { 
        return basic->getParameters();
    }

    void setParameters(std::vector<std::shared_ptr<Type>> parameters) override {
        basic->setParameters(parameters);
    }

    bool isPtr() override { return true; }

    bool isRef() override { return true; }

    std::shared_ptr<Type> getElementTy() override {
        return basic;
    }

    std::string toString() override {
        return "'" + basic->toString();
    }

    std::string hash() override {
        return "p" + basic->hash();
    }
};
}


#endif