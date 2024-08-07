#include "Type.h"


namespace wind {

std::shared_ptr<Type> getType(std::string name, std::vector<std::shared_ptr<Type>> parameters) {
    if (parameters.empty()) {
        if (name == "void") return Type::VOID;
        if (name == "ptr") return Type::PTR;
        if (name == "bool") return Type::BOOL;
        if (name == "i8") return Type::I8;
        if (name == "i16") return Type::I16;
        if (name == "i32") return Type::I32;
        if (name == "i64") return Type::I64;
        if (name == "u8") return Type::U8;
        if (name == "u16") return Type::U16;
        if (name == "u32") return Type::U32;
        if (name == "u64") return Type::U64;
        if (name == "f32") return Type::F32;
        if (name == "f64") return Type::F64;
        return std::make_shared<NamedType>(name, parameters);
    } else {
        return std::make_shared<NamedType>(name, parameters);
    }
}
}
