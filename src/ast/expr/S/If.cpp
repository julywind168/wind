#include "If.h"


void If::markTy(std::shared_ptr<wind::Type> ty) {
    panic("if type depend on `then` and `else` expr, can't been mark");
}

void If::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    thenEnv = std::make_shared<Env>(env);
    _test->typecheck(env, wind::Type::BOOL);
    _then->typecheck(thenEnv);
    if (_else) {
        elseEnv = std::make_shared<Env>(env);
        _else->typecheck(elseEnv);
        if (isEqual(_then->ty, _else->ty)) {
            this->ty = _then->ty;
        } else {
            panic("if `then` and `else` expr type not equal");
        }
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("If::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* If::codegen(CompileCtx &ctx) {
    llvm::Value* cond = _test->codegen(ctx);
    if (!cond) {
        panic("If::codegen failed, test expr codegen failed");
    }
    llvm::Function *f = ctx.builder->GetInsertBlock()->getParent();
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*ctx.context, "then", f);
    llvm::BasicBlock* elseBB = llvm::BasicBlock::Create(*ctx.context, "else");
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*ctx.context, "ifcont");
    ctx.builder->CreateCondBr(cond, thenBB, elseBB);

    // emit then value
    ctx.builder->SetInsertPoint(thenBB);
    llvm::Value* thenVal = _then->codegen(ctx);
    if (!thenVal) {
        panic("If::then branch did not return a value");
    }
    ctx.builder->CreateBr(mergeBB);
    // Codegen of 'then' can change the current block, update thenBB for the PHI.
    thenBB = ctx.builder->GetInsertBlock();

    // emit else value
    f->insert(f->end(), elseBB);
    ctx.builder->SetInsertPoint(elseBB);
    llvm::Value* elseVal;
    if (_else) {
        elseVal = _else->codegen(ctx);
        if (!elseVal) {
            panic("If::else branch did not return a value");
        }
    }
    ctx.builder->CreateBr(mergeBB);
    // Codegen of 'else' can change the current block, update elseBB for the PHI.
    elseBB = ctx.builder->GetInsertBlock();

    // emit merge block
    f->insert(f->end(), mergeBB);
    ctx.builder->SetInsertPoint(mergeBB);

    if (ty->isVoid()) {
        return nullptr;
    } else {
        llvm::PHINode* node = ctx.builder->CreatePHI(ctx.getTy(ty), 2, "iftmp");
        node->addIncoming(thenVal, thenBB);
        node->addIncoming(elseVal, elseBB);
        return node;
    }
}
