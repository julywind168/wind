#include "Load.h"


void Load::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    if (valTy->isVoid()) {
        panic("Cannot load a void value");
    }
    address->typecheck(env, wind::Type::PTR);
    if (offset) {
        offset->typecheck(env, wind::Type::I32);
    }
    ty = valTy;
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Load::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Load::codegen(CompileCtx &ctx) {
    llvm::Value* addr = nullptr;
    if (offset) {
        addr = ctx.builder->CreateInBoundsGEP(ctx.getTy(ty), address->codegen(ctx), offset->codegen(ctx));
    } else {
        addr = address->codegen(ctx);
    }
    return ctx.builder->CreateLoad(ctx.getTy(ty), addr, "tmpload");
}