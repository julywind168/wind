#include "FloatLit.h"


void FloatLit::markTy(std::shared_ptr<wind::Type> ty) {
    if (!ty->isFloat()) {
        panic("FloatLit only accept `f32` or `f64`, but got " + ty->toString());
    }
    this->ty = ty;
    marked = true;
}

void FloatLit::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    if (expectedTy && !isEqual(ty, expectedTy)) {
        if (marked) {
            panic("FloatLit cannot cast from " + ty->toString() + " to " + expectedTy->toString());
        } else {
            // conditional auto cast 
            if (expectedTy->isFloat()) {
                ty = expectedTy;
            } else {
                panic(fmt::format("FloatLit::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
            }
        }
    }
}

llvm::Value* FloatLit::codegen(CompileCtx &ctx) {
    return llvm::ConstantFP::get(ctx.getTy(ty), value);
}
