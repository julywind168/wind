#include "Impl.h"

void Impl::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Impl type is void, can't been mark");
}

void Impl::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    for (auto &func : funcs) {
        func->typecheck(env);
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Impl::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Impl::codegen(CompileCtx &ctx, bool enableDeRef) {
    for (auto &func : funcs) {
        func->codegen(ctx);
    }
    return nullptr;
}
