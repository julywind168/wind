#ifndef WIND_TYPE_INTERFACE_H
#define WIND_TYPE_INTERFACE_H

#include <string>
#include <map>

namespace wind {

class Type : public std::enable_shared_from_this<Type> {
public:
    virtual ~Type() = default;
    virtual std::shared_ptr<Type> get() { return shared_from_this(); }
    virtual std::shared_ptr<Type> getElementTy() { return nullptr; }
    virtual std::string getName() { return ""; }
    virtual std::vector<std::shared_ptr<Type>> getParameters() { return {}; }
    virtual void setParameters(std::vector<std::shared_ptr<Type>> parameters) { }
    virtual std::string toString() = 0;
    virtual std::string hash() { return toString(); }
    virtual bool isVoid() { return false; }
    virtual bool isVoidPtr() { return false; }
    virtual bool isPtr() { return false; }
    virtual bool isRef() { return false; }
    virtual bool isBool() { return false; }
    virtual bool isSigned() { return false; }
    virtual bool isUnsigned() { return false; }
    virtual bool isInteger() { return isSigned() || isUnsigned(); }
    virtual bool isFloat() { return false; }
    bool isNumber() { return isInteger() || isFloat(); }
    virtual bool isPrimitive() { return false; }
    virtual bool isStruct() { return false; }
    virtual bool isProtocol() { return false; }

    std::shared_ptr<Type> ref();
    bool equalTo(std::shared_ptr<Type> other) {
        return hash() == other->hash();
    }

    static std::shared_ptr<Type> VOID;
    static std::shared_ptr<Type> BOOL;
    static std::shared_ptr<Type> PTR;
    static std::shared_ptr<Type> I8;
    static std::shared_ptr<Type> I16;
    static std::shared_ptr<Type> I32;
    static std::shared_ptr<Type> I64;
    static std::shared_ptr<Type> U8;
    static std::shared_ptr<Type> U16;
    static std::shared_ptr<Type> U32;
    static std::shared_ptr<Type> U64;
    static std::shared_ptr<Type> F32;
    static std::shared_ptr<Type> F64;
};
}


#endif