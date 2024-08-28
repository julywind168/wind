#include "Block.h"
#include "Identifier.h"

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

llvm::Value* Block::codegen(CompileCtx &ctx) {
    llvm::Value* ret = nullptr;
    std::string retVarName = "";
    for (auto& expr : exprs) {
        ret = expr->codegen(ctx);
        if (ret && expr->nodeTy() == NodeType::IDENTIFIER) {
            retVarName = ((Identifier*)(expr.get()))->getIdent();
        }
    }
    myEnv->onClose(ctx, retVarName);
    return ret;
}
