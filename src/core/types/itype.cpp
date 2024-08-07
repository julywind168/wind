#include "itype.h"
#include "primitives.h"
#include "quote.h"

namespace wind {

std::shared_ptr<Type> Type::toQuote() {
    return std::make_shared<QuoteType>(shared_from_this());
}

std::shared_ptr<Type> Type::VOID(new VoidType());
std::shared_ptr<Type> Type::BOOL(new BoolType());
std::shared_ptr<Type> Type::PTR(new PtrType());
std::shared_ptr<Type> Type::I8(new I8Type());
std::shared_ptr<Type> Type::I16(new I16Type());
std::shared_ptr<Type> Type::I32(new I32Type());
std::shared_ptr<Type> Type::I64(new I64Type());
std::shared_ptr<Type> Type::U8(new U8Type());
std::shared_ptr<Type> Type::U16(new U16Type());
std::shared_ptr<Type> Type::U32(new U32Type());
std::shared_ptr<Type> Type::U64(new U64Type());
std::shared_ptr<Type> Type::F32(new F32Type());
std::shared_ptr<Type> Type::F64(new F64Type());
}