#include "IntLit.h"


void IntLit::markTy(std::shared_ptr<wind::Type> ty) {
    if (!ty->isInteger()) {
        panic("IntLit only accept `i8 i16 i32 i64` or `u8 u16 u32 u64`, but got " + ty->toString());
    }
    this->ty = ty;
    marked = true;
}

void IntLit::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    if (expectedTy && !isEqual(ty, expectedTy)) {
        if (marked) {
            panic("IntLit cannot cast from " + ty->toString() + " to " + expectedTy->toString());
        } else {
            // conditional auto cast 
            if (expectedTy->isInteger() || expectedTy->isFloat()) {
                ty = expectedTy;
            } else {
                panic(fmt::format("IntLit::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
            }
        }
    }
}

llvm::Value* IntLit::codegen(CompileCtx &ctx) {
    return llvm::ConstantInt::get(ctx.getTy(ty), value, ty->isSigned());
}
