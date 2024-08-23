#include "Alloc.h"

void Alloc::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    if (size) {
        if (size->isLiteral()) {
            size->markTy(wind::Type::U64);
        }
        size->typecheck(env, wind::Type::U64);
    }
    ty = wind::Type::PTR;
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Alloc::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Alloc::codegen(CompileCtx &ctx) {
    auto sz = size ? size->codegen(ctx) : nullptr;
    return ctx.builder->CreateAlloca(ctx.getTy(basicTy), sz, "ptr");
}
