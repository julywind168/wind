#include "Alloc.h"

void Alloc::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    if (size->isLiteral()) {
        size->markTy(wind::Type::U64);
    }
    size->typecheck(env, wind::Type::U64);
    ty = std::make_shared<wind::ReferenceType>(basicTy);
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Alloc::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Alloc::codegen(CompileCtx &ctx) {
    auto sz = size->codegen(ctx);
    return ctx.builder->CreateAlloca(ctx.getTy(basicTy), sz, "ptr");
}
