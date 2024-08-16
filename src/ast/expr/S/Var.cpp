#include "Var.h"
#include "Protocol.h"

void Var::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Var/Let has default `void` type, can't been mark");
}

void Var::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    this->env = env;
    init->typecheck(env);
    if (init->ty == nullptr) {
        panic("Var::typecheck init expr type is void");
    }
    if (hintTy == nullptr) {
        hintTy = init->ty;
    }
    if (invalidTypeCast(env, init->ty, hintTy)) {
        panic(fmt::format("Var::typecheck failed, expected {}, but {}", hintTy->toString(), init->ty->toString()));
    }
    symbol = Symbol::variable_(name, hintTy, const_, nullptr);
    env->define(name, symbol);
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Var::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Var::codegen(CompileCtx &ctx, bool enableDeRef) {
    llvm::AllocaInst *address = ctx.builder->CreateAlloca(ctx.getTy(hintTy), nullptr, name);
    symbol->v.address = address;
    llvm::Value* value = init->codegen(ctx);
    // cast 'struct to protocol
    if (!isEqual(hintTy, init->ty) && hintTy->isProtocol()) {
        auto structTy = ctx.getStructTy(hintTy->isPtr() ? hintTy->getElementTy() : hintTy);
        auto s = env->lookup(hintTy->toString());
        if (!s || s->ty != SymbolType::PROTOCOL) {
            panic("Protocol not found");
        }
        llvm::Value *vTable = s->p->getvTable(ctx, init->ty);
        ctx.builder->CreateStore(value, ctx.builder->CreateStructGEP(structTy, address, 0));
        ctx.builder->CreateStore(vTable, ctx.builder->CreateStructGEP(structTy, address, 1));
        return address;
    }
    return ctx.builder->CreateStore(value, address);
}
