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

// `root[key] = value` => (root.__newindex__ key value)
std::unique_ptr<Expr> Equal::createNewIndexCall() {
    auto isNewIndexCall = [&](std::string tyName) {
        if (auto s = env->lookup(tyName)) {
            if(s->ty == SymbolType::TYPE) {
                if (s->t.findMethod("__newindex__")) {
                    return true;
                }
            }
        }
        return false;
    };

    if (left->nodeTy() == NodeType::INDEX) {
        auto l = (Index*)left.get();
        if (isNewIndexCall(l->root->ty->getName())) {
            std::string code = fmt::format("({}.__newindex__ {} {})", l->root->getSourceCode(), l->key->getSourceCode(), right->getSourceCode());
            return parseString(env, "__newindex__", code);
        }
    }
    return nullptr;
}

llvm::Value* Equal::codegen(CompileCtx &ctx, bool enableDeRef) {
    if (auto call = createNewIndexCall()) {
        call->typecheck(env, ty);
        return call->codegen(ctx);
    } else {
        auto address = (llvm::AllocaInst*)left->getVariableAddress(ctx);
        auto value = right->codegen(ctx);
        return ctx.builder->CreateStore(value, address);
        return llvm::ConstantInt::get(ctx.getTy(ty), true);
    }
}
