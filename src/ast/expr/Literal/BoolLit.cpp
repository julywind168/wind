#include "BoolLit.h"


void BoolLit::markTy(std::shared_ptr<wind::Type> ty) {
    panic("BoolLit has default bool type, can't been mark");
}

llvm::Value* BoolLit::codegen(CompileCtx &ctx) {
    return llvm::ConstantInt::get(ctx.getTy(ty), value);
}
