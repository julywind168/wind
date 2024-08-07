#ifndef WIND_TYPE_PRIMITIVES_H
#define WIND_TYPE_PRIMITIVES_H

#include "itype.h"

namespace wind {


class PrimitiveType : public Type {
public:
    PrimitiveType() {}
    bool isPrimitive() override { return true; }
    std::string getName() override { return toString(); }
};


class VoidType : public PrimitiveType {
public:
    VoidType() {}
    bool isVoid() override { return true; }
    std::string toString() override { return "void"; }
};


class BoolType : public PrimitiveType {
public:
    BoolType() {}
    bool isBool() override { return true; }
    std::string toString() override { return "bool"; }
};


class PtrType : public PrimitiveType {
public:
    PtrType() {}
    bool isVoidPtr() override { return true; }
    bool isPtr() override { return true; }
    std::string toString() override { return "ptr"; }
};


class I8Type : public PrimitiveType {
public:
    I8Type() {}
    bool isSigned() override { return true; }
    std::string toString() override { return "i8"; }
};


class I16Type : public PrimitiveType {
public:
    I16Type() {}
    bool isSigned() override { return true; }
    std::string toString() override { return "i16"; }
};


class I32Type : public PrimitiveType {
public:
    I32Type() {}
    bool isSigned() override { return true; }
    std::string toString() override { return "i32"; }
};


class I64Type : public PrimitiveType {
public:
    I64Type() {}
    bool isSigned() override { return true; }
    std::string toString() override { return "i64"; }
};


class U8Type : public PrimitiveType {
public:
    U8Type() {}
    bool isUnsigned() override { return true; }
    std::string toString() override { return "u8"; }
};


class U16Type : public PrimitiveType {
public:
    U16Type() {}
    bool isUnsigned() override { return true; }
    std::string toString() override { return "u16"; }
};


class U32Type : public PrimitiveType {
public:
    U32Type() {}
    bool isUnsigned() override { return true; }
    std::string toString() override { return "u32"; }
};


class U64Type : public PrimitiveType {
public:
    U64Type() {}
    bool isUnsigned() override { return true; }
    std::string toString() override { return "u64"; }
};


class F32Type : public PrimitiveType {
public:
    F32Type() {}
    bool isFloat() override { return true; }
    std::string toString() override { return "f32"; }
};


class F64Type : public PrimitiveType {
public:
    F64Type() {}
    bool isFloat() override { return true; }
    std::string toString() override { return "f64"; }
};
}


#endif