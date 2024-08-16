#include "Block.h"

void Block::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    myEnv = std::make_shared<Env>(env);
    for (auto& expr : exprs) {
        expr->typecheck(myEnv);
    }
    if (exprs.size() > 0) {
        ty = exprs.back()->ty;
    } else {
        panic("Empty block");
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Call::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Block::codegen(CompileCtx &ctx, bool enableDeRef) {
    llvm::Value* ret = nullptr;
    for (auto& expr : exprs) {
        ret = expr->codegen(ctx);
    }
    return ret;
}
