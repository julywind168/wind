#include "Index.h"
#include "ast/expr/Literal/IntLit.h"

void Index::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Index type depend on `root` and `key`, can't been mark");
}

void Index::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    this->env = env;
    root->typecheck(env);
    key->typecheck(env, wind::Type::I32);
    if (auto m = env->lookupMeatFunc(root->ty->getName(), "__index")) {
        ty = m->getCallee_()->retTy;
    } else {
        panic("Index::typecheck failed, can't find method `__index`");
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Index::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}


// root[key] => (root.__index key)
std::unique_ptr<Expr> Index::createIndexCall() {
    std::string code = "(" + root->getSourceCode() + ".__index " + key->getSourceCode() + ")";
    return parseString(env, "__index", code);
}

llvm::Value* Index::codegen(CompileCtx &ctx) {
    auto call = createIndexCall();
    call->typecheck(env, ty);
    return call->codegen(ctx);
}
