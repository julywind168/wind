#ifndef WIND_TYPE_NAMED_H
#define WIND_TYPE_NAMED_H

#include "itype.h"

namespace wind {


extern std::unordered_map<std::string, int> NAMED;
extern std::unordered_map<std::string, std::vector<std::shared_ptr<Type>>> STRUCT;


class NamedType : public Type {
public:
    std::string name;
    std::vector<std::shared_ptr<Type>> parameters;

    NamedType(std::string name, std::vector<std::shared_ptr<Type>> parameters = {})
        : name(name), parameters(parameters) {}

    std::string getName() override {
        return name;
    }

    std::vector<std::shared_ptr<Type>> getParameters() override { 
        return parameters;
    }

    void setParameters(std::vector<std::shared_ptr<Type>> parameters) override {
        this->parameters = parameters;
    }

    bool isStruct() override {
        if (NAMED.count(name) == 0) {
            throw std::runtime_error("Undefined named type: " + name);
        }
        return NAMED[name] == 0;
    }

    bool isProtocol() override {
        if (NAMED.count(name) == 0) {
            throw std::runtime_error("Undefined named type: " + name);
        }
        return NAMED[name] == 1;
    }

    bool isGeneric() {
        return parameters.size() > 0;
    }

    std::string toString() override {
        auto str = name;
        if (parameters.size() > 0) {
            str += "<";
            for (auto i = 0; i < parameters.size(); i++) {
                str += parameters[i]->toString();
                if (i < parameters.size() - 1) {
                    str += ", ";
                }
            }
            str += ">";
        }
        return str;
    }

    std::string hash() override {
        auto str = name;
        for (auto i = 0; i < parameters.size(); i++) {
            str += ("#" + parameters[i]->hash());
        }
        return str;
    }

    static void defineStruct(std::string name, std::vector<std::shared_ptr<Type>> fields) {
        NAMED[name] = 0;
        STRUCT[name] = fields;
    }

    static std::vector<std::shared_ptr<Type>> getStructFieldTys(std::string name) {
        return STRUCT[name];
    }

    static void defineProtocol(std::string name) {
        NAMED[name] = 1;
    }
};
}


#endif