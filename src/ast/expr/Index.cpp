#include "Index.h"
#include "ast/expr/Literal/IntLit.h"

void Index::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Index type depend on `root` and `key`, can't been mark");
}

void Index::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    this->env = env;
    root->typecheck(env);
    key->typecheck(env, wind::Type::I32);
    // root[key] => (root.__index key)
    if (env->lookupMeatFunc(root->ty->getName(), "__index")) {
        std::string code = "(" + root->getSourceCode() + ".__index " + key->getSourceCode() + ")";
        call = parseString(env, "__index", code);
        call->typecheck(env, expectedTy);
        ty = call->ty;
    } else {
        panic("Index::typecheck failed, can't find method `__index`");
    }
}

llvm::Value* Index::codegen(CompileCtx &ctx) {
    return call->codegen(ctx);
}
