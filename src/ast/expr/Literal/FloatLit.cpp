#include "FloatLit.h"

bool isFloatRef(std::shared_ptr<wind::Type> ty) {
    return ty->isPtr() && ty->getElementTy() && ty->getElementTy()->isFloat();
}

void FloatLit::markTy(std::shared_ptr<wind::Type> ty) {
    if (!ty->isFloat() && !isFloatRef(ty)) {
        panic("FloatLit only accept `f32`, `f64` or `'f32` `'f64`, but got " + ty->toString());
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

llvm::Value* FloatLit::codegen(CompileCtx &ctx, bool enableDeRef) {
    auto val = llvm::ConstantFP::get(ctx.getTy(ty), value);
    if (ty->isPtr()) {
        auto inst = ctx.mallocInstance(ctx.getTy(ty), "ptr");
        ctx.builder->CreateStore(val, inst);
        return inst;
    } else {
        return val;
    }
}
