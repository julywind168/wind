#include "Index.h"
#include "ast/expr/Literal/IntLit.h"

void Index::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Index type depend on `root` and `key`, can't been mark");
}

void Index::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    this->env = env;
    root->typecheck(env);
    key->typecheck(env, wind::Type::I32);
    if (auto s = env->lookup(root->ty->getName())) {
        if(s->ty == SymbolType::TYPE) {
            if (auto m = s->t.findMethod("__index__")) {
                ty = m->getCallee_()->retTy;
                isIndexCall = true;
            }
        }
    }
    if (!isIndexCall) {
        ty = root->ty->getElementTy();
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Index::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Index::getVariableAddress(CompileCtx &ctx) {
    return ctx.builder->CreateInBoundsGEP(ctx.getTy(ty), root->codegen(ctx), key->codegen(ctx));
}


// root[key] => (root.__index__ key)
std::unique_ptr<Expr> Index::createIndexCall() {
    std::string code = "(" + root->getSourceCode() + ".__index__ " + key->getSourceCode() + ")";
    return parseString(env, "__index__", code);
}

llvm::Value* Index::codegen(CompileCtx &ctx) {
    if (isIndexCall) {
        auto call = createIndexCall();
        call->typecheck(env, ty);
        return call->codegen(ctx);
    } else {
        auto addr = getVariableAddress(ctx);
        return ctx.builder->CreateLoad(ctx.getTy(ty), addr);
    }
}
