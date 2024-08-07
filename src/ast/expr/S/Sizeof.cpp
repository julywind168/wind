#include "Sizeof.h"

void Sizeof::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Sizeof has default u64 type, can't been mark");
}

void Sizeof::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    ty = wind::Type::U64;
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Sizeof::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Sizeof::codegen(CompileCtx &ctx) {
    return ctx.builder->getInt64(ctx.getTypeSize(ctx.getTy(type)));
}
