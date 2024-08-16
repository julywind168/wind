#include "Loop.h"

void Loop::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Loop has default `void` type, can't been mark");
}

void Loop::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    myEnv = std::make_shared<Env>(env);
    if (init)
        init->typecheck(myEnv);
    test->typecheck(myEnv, wind::Type::BOOL);
    body->typecheck(myEnv);
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Loop::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Loop::codegen(CompileCtx &ctx, bool enableDeRef) {
    if (init) init->codegen(ctx);

    llvm::Function *f = ctx.builder->GetInsertBlock()->getParent();
    llvm::BasicBlock *testBB = llvm::BasicBlock::Create(*ctx.context, "loop.test", f);
    llvm::BasicBlock *bodyBB = llvm::BasicBlock::Create(*ctx.context, "loop.body");
    llvm::BasicBlock *endBB = llvm::BasicBlock::Create(*ctx.context, "loop.end");
    ctx.builder->CreateBr(testBB);

    // test
    ctx.builder->SetInsertPoint(testBB);
    llvm::Value *cond = test->codegen(ctx);
    if (!cond)
        return nullptr;
    ctx.builder->CreateCondBr(cond, bodyBB, endBB);
    testBB = ctx.builder->GetInsertBlock();
    f->insert(f->end(), bodyBB);

    // body
    ctx.builder->SetInsertPoint(bodyBB);
    body->codegen(ctx);
    ctx.builder->CreateBr(testBB);
    bodyBB = ctx.builder->GetInsertBlock();
    f->insert(f->end(), endBB);

    // end
    ctx.builder->SetInsertPoint(endBB);
    return nullptr;
}
