#include "BoolLit.h"

bool isBoolRef(std::shared_ptr<wind::Type> ty) {
    return ty->isPtr() && ty->getElementTy() && ty->getElementTy()->isBool();
}

void BoolLit::markTy(std::shared_ptr<wind::Type> ty) {
    if (!ty->isBool() && !isBoolRef(ty)) {
        panic("FloatLit only accept `bool` or `'bool`, but got " + ty->toString());
    }
    this->ty = ty;
}

llvm::Value* BoolLit::codegen(CompileCtx &ctx, bool enableDeRef) {
    auto val = llvm::ConstantInt::get(ctx.getTy(ty), value);
    if (ty->isPtr()) {
        auto inst = ctx.mallocInstance(ctx.getTy(ty), "ptr");
        ctx.builder->CreateStore(val, inst);
        return inst;
    } else {
        return val;
    }
}
