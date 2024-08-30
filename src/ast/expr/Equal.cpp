#include "Equal.h"
#include "Index.h"
#include "Identifier.h"


void Equal::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Equal has default type `void`, can't been mark");
}

void Equal::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    this->env = env;
    left->typecheck(env);
    right->typecheck(env);
    // replace right => (right.__clone)
    if (right->isIdentifier() && env->lookupMeatFunc(right->ty->getName(), "__clone")) {
        right = parseString(env, "__clone", fmt::format("({}.__clone)", right->getSourceCode()));
        right->typecheck(env);
    }
    if (left->nodeTy() != NodeType::IDENTIFIER && left->nodeTy() != NodeType::DOT && left->nodeTy() != NodeType::INDEX) {
        panic("Equal::typecheck failed, left side must be an identifier (current version)");
    }
    if (left->nodeTy() == NodeType::IDENTIFIER) {
        auto s = ((Identifier*)(left.get()))->getSymbol();
        if (s->v.const_) {
            panic("Equal:: left is a const variable, can't been changed");
        }
    }
    if (invalidTypeCast(env, right->ty, left->ty)) {
        panic(fmt::format("Equal::typecheck failed, left({}) and right({}) types dismatch", left->ty->toString(), right->ty->toString()));
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Equal::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

// `root[key] = value` => (root.__newindex key value)
std::unique_ptr<Expr> Equal::createNewIndexCall() {
    if (left->nodeTy() == NodeType::INDEX) {
        auto l = (Index*)left.get();
        if (env->lookupMeatFunc(l->root->ty->getName(), "__newindex")) {
            std::string code = fmt::format("({}.__newindex {} {})", l->root->getSourceCode(), l->key->getSourceCode(), right->getSourceCode());
            return parseString(env, "__newindex", code);
        }
    }
    return nullptr;
}

llvm::Value* Equal::codegen(CompileCtx &ctx) {
    if (auto call = createNewIndexCall()) {
        call->typecheck(env, ty);
        return call->codegen(ctx);
    } else {
        auto address = (llvm::AllocaInst*)left->getVariableAddress(ctx);
        auto value = right->codegen(ctx);
        return ctx.builder->CreateStore(value, address);
    }
}
