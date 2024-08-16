#include "IntLit.h"

bool isIntergerRef(std::shared_ptr<wind::Type> ty) {
    return ty->isPtr() && ty->getElementTy() && ty->getElementTy()->isInteger();
}

void IntLit::markTy(std::shared_ptr<wind::Type> ty) {
    if (!ty->isInteger() && !isIntergerRef(ty)) {
        panic("IntLit only accept interger or ref of interger, but got " + ty->toString());
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

llvm::Value* IntLit::codegen(CompileCtx &ctx, bool enableDeRef) {
    auto val = llvm::ConstantInt::get(ctx.getTy(ty), value, ty->isPtr() ? ty->getElementTy()->isSigned() : ty->isSigned());
    if (ty->isPtr()) {
        auto inst = ctx.mallocInstance(ctx.getTy(ty), "ptr");
        ctx.builder->CreateStore(val, inst);
        return inst;
    } else {
        return val;
    }
}
