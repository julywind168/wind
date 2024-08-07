#include "Cond.h"
#include "ast/expr/Literal/BoolLit.h"

void Cond::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Cond type depend on sub exprs, can't been mark");
}

void Cond::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    size_t idx = 0;
    ty = nullptr;
    for (auto &[test, then]: cases) {
        idx++;
        test->typecheck(env, wind::Type::BOOL);
        then->typecheck(env, ty);
        ty = then->ty;
        // last one must be `true => expr`
        if (idx == cases.size()) {
            if (test->nodeTy() == NodeType::BOOL_LIT && test->toString() == "true") {
                // pass
            } else {
                panic("cond last one case must be `true => expr` `");
            }
        }
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Call::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Cond::codegen(CompileCtx &ctx) {
    llvm::Function *f = ctx.builder->GetInsertBlock()->getParent();
    llvm::BasicBlock *mergeBB = llvm::BasicBlock::Create(*ctx.context, "cond.cont");
    std::vector<llvm::BasicBlock *> testBlocks;
    std::vector<llvm::BasicBlock *> thenBlocks;
    std::vector<llvm::Value*> thenVals;

    size_t size = cases.size();

    auto nextTestBB = [size, &testBlocks, &mergeBB](size_t idx) {
        if (idx < size - 1) {
            return testBlocks[idx + 1];
        } else {
            return mergeBB;
        }
    };
    for (size_t i = 0; i < size; ++i) {
        testBlocks.push_back(llvm::BasicBlock::Create(*ctx.context, "cond.test" + std::to_string(i)));
        thenBlocks.push_back(llvm::BasicBlock::Create(*ctx.context, "cond.then" + std::to_string(i)));
    }
    ctx.builder->CreateBr(testBlocks[0]);

    for (size_t i = 0; i < size; ++i) {
        auto testdBB = testBlocks[i];
        auto thenBB = thenBlocks[i];
        f->insert(f->end(), testdBB);
        ctx.builder->SetInsertPoint(testdBB);
        llvm::Value *cond = cases[i].first->codegen(ctx);
        ctx.builder->CreateCondBr(cond, thenBB, nextTestBB(i));
        // body
        f->insert(f->end(), thenBB);
        ctx.builder->SetInsertPoint(thenBB);
        auto v = cases[i].second->codegen(ctx);
        if (!ty->isVoid()) {
            assert(v);
            thenVals.push_back(v);
        }
        ctx.builder->CreateBr(mergeBB);
    }

    // emit merge block
    f->insert(f->end(), mergeBB);
    ctx.builder->SetInsertPoint(mergeBB);
    if (ty->isVoid()) {
        return nullptr;
    } else {
        llvm::PHINode* node = ctx.builder->CreatePHI(ctx.getTy(ty), size, "condtmp");
        for (size_t i = 0; i < size; i++) {
            node->addIncoming(thenVals[i], thenBlocks[i]);
        }
        return node;
    }
}
