#include "Store.h"


void Store::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    value->typecheck(env);
    address->typecheck(env, wind::Type::PTR);
    if (offset) {
        offset->typecheck(env, wind::Type::I32);
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Store::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Store::codegen(CompileCtx &ctx) {
    llvm::Value* addr = nullptr;
    if (offset) {
        addr = ctx.builder->CreateInBoundsGEP(ctx.getTy(value->ty), address->codegen(ctx), offset->codegen(ctx));
    } else {
        addr = address->codegen(ctx);
    }
    return ctx.builder->CreateStore(value->codegen(ctx), addr);
}