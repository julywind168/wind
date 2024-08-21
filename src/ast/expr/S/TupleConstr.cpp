#include "TupleConstr.h"


void TupleConstr::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    std::vector<std::shared_ptr<wind::Type>> parameters;
    for (auto &e : elements) {
        e->typecheck(env);
        parameters.push_back(e->ty);
    }
    ty = wind::getType("tuple", parameters);
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("TupleConstr::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* TupleConstr::codegen(CompileCtx &ctx) {
    llvm::StructType* structTy = ctx.getStructTy(ty);
    llvm::Value *instance = ctx.builder->CreateAlloca(structTy, nullptr, "tuple");
    for (size_t i = 0; i < elements.size(); i++) {
        llvm::Value *val = elements[i]->codegen(ctx);
        llvm::Value *addr = ctx.builder->CreateStructGEP(structTy, instance, i);
        ctx.builder->CreateStore(val, addr);
    }
    return ctx.builder->CreateLoad(structTy, instance, "tuple");
}