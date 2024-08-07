#include "Func.h"

void Func::markTy(std::shared_ptr<wind::Type> ty) {
    panic("func expr can't been mark");
}

void Func::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    for (auto& p : callee->params) {
        myEnv->define(p.name, Symbol::variable_(p.name, p.ty, false, nullptr));
    }
    // tips: body typecheck in codegen
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Func::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Function* Func::findInstance(std::vector<std::shared_ptr<wind::Type>> types) {
    for (auto &i : instances) {
        if (isEqual(i.types, types)) {
            return i.f;
        }
    }
    return nullptr;
}

llvm::Function* Func::genfunc(CompileCtx &ctx, std::vector<std::shared_ptr<wind::Type>> types) {
    if (auto f = findInstance(types)) {
        return f;
    }
    auto env = std::make_shared<Env>(myEnv);
    callee->switchTypes(types);
    body->typecheck(env);
    auto f = callee->codegen(ctx);
    llvm::BasicBlock *bb = llvm::BasicBlock::Create(*ctx.context, "entry", f);
    ctx.builder->SetInsertPoint(bb);

    size_t idx = 0;
    for(auto &arg: f->args()) {
        auto name = callee->params[idx++].name;
        llvm::AllocaInst *addr = ctx.builder->CreateAlloca(arg.getType(), nullptr, name);
        auto s = env->lookup(name);
        if (!s || s->ty != SymbolType::VARIABLE) {
            panic(name + " not a variable");
        }
        s->v.address = addr;
        ctx.builder->CreateStore(&arg, addr);
    }
    auto ret = body->codegen(ctx);

    if (callee->retTy->isVoid()) {
        ctx.builder->CreateRetVoid();
    } else {
        if (ret == nullptr) {
            panic("return type mismatch");
        }
        ctx.builder->CreateRet(ret);
    }

    if (llvm::verifyFunction(*f, &llvm::errs())) {
        panic("verify function failed");
    }
    if (ctx.options.optimize) {
        ctx.theFPM->run(*f, *ctx.theFAM);
    }
    // insert to instances
    instances.push_back({env, types, f});
    return f;
}

llvm::Value* Func::codegen(CompileCtx &ctx) {
    if (callee->generics.empty()) {
        return genfunc(ctx, {});
    }
    return nullptr;
}
