#include "Identifier.h"
#include "S/Func.h"

void Identifier::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    this->env = env;
    symbol = this->env->lookup(name);
    if (!symbol) {
        panic(fmt::format("Identifier::typecheck failed, symbol {} not found", name));
    }
    if (symbol && symbol->ty == SymbolType::VARIABLE) {
        ty = symbol->v.ty;
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Identifier::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

std::shared_ptr<Callee> Identifier::getCallee(std::vector<std::shared_ptr<wind::Type>> types) {
    symbol = env->lookup(name);
    if (symbol->ty == SymbolType::FUNCTION) {
        auto callee = symbol->f->getCallee_();
        callee->switchTypes(types);
        return callee;
    } else {
        panic("Identifier::getCallee: " + name + " is not a function");
        return nullptr;
    }
}

llvm::Value* Identifier::createCall(CompileCtx &ctx, std::vector<std::shared_ptr<wind::Type>> types, std::vector<llvm::Value*> args) {
    if (symbol->ty == SymbolType::FUNCTION) {
        auto curBB = ctx.builder->GetInsertBlock();
        auto f = symbol->f->genfunc(ctx, types);
        ctx.builder->SetInsertPoint(curBB);
        return ctx.builder->CreateCall(f, args);
    } else {
        panic("Identifier " + name + " is not a function");
        return nullptr;
    }
}

llvm::Value* Identifier::codegen(CompileCtx &ctx) {
    return ctx.builder->CreateLoad(ctx.getTy(ty), symbol->v.address, name);
}
