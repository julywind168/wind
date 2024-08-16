#include "Dot.h"
#include "S/Protocol.h"

void Dot::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Dot type depend on `root` and `key`, can't been mark");
}

void Dot::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    this->env = env;
    root->typecheck(env);
    if (root->ty->getName() == "tuple") {
        auto idx = std::stoi(key);
        auto parameters = root->ty->getParameters();
        if (idx >= parameters.size()) {
            panic("tuple index out of range");
        }
        ty = parameters[idx];
    } else {
        auto s = env->lookup(root->ty->getName());
        if (!s) {
            panic("Can't find type: " + root->ty->getName());
        }
        for (auto& field : s->t.fields) {
            if (field.name == key) {
                ty = field.ty;
                break;
            }
        }
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Dot::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

std::shared_ptr<Callee> Dot::getCallee(std::vector<std::shared_ptr<wind::Type>> _types) {
    root->typecheck(env);
    auto s = env->lookup(root->ty->getName());
    if (s && s->ty == SymbolType::TYPE) {
        return s->queryType(root->ty->getParameters())->findMethod(key)->getCallee_();
    }
    if (s && s->ty == SymbolType::PROTOCOL) {
        return s->p->findMethod(key);
    }
    panic("Dot::getCallee failed, undefined callee " + key);
    return nullptr;
}

llvm::Value* Dot::createCall(CompileCtx &ctx, std::vector<std::shared_ptr<wind::Type>> _types, std::vector<llvm::Value*> args) {
    auto s = env->lookup(root->ty->getName());
    if (s->ty == SymbolType::TYPE) {
        auto m = s->t.findMethod(key);

        // generate f and back to current BB
        auto curBB = ctx.builder->GetInsertBlock();
        auto f = m->genfunc(ctx, root->ty->getParameters());
        ctx.builder->SetInsertPoint(curBB);

        auto self = root->ty->isStruct() ? root->getVariableAddress(ctx) : root->codegen(ctx);
        args.insert(args.begin(), self);
        return ctx.builder->CreateCall(f, args);
    } else if (s->ty == SymbolType::PROTOCOL) {
        auto index = s->p->getMethodIndex(key);
        auto fnTy = s->p->getMethodFnTy(ctx, key);
        auto instance = root->getVariableAddress(ctx);
        auto self = ctx.builder->CreateLoad(ctx.getVoidPtrTy(), ctx.builder->CreateStructGEP(ctx.getProtocolStructTy(), instance, 0), "self");
        auto vTable = ctx.builder->CreateLoad(ctx.getVoidPtrTy(), ctx.builder->CreateStructGEP(ctx.getProtocolStructTy(), instance, 1), "vtable");
        auto VTableTy = s->p->getvTableType(ctx);
        auto f = ctx.builder->CreateLoad(ctx.getVoidPtrTy(), ctx.builder->CreateStructGEP(VTableTy, vTable, index), "f");
        args.insert(args.begin(), self);
        return ctx.builder->CreateCall(fnTy, f, args);
    }
    return nullptr;
}

llvm::Value* Dot::getVariableAddress(CompileCtx &ctx, bool enableDeRef) {
    if (root->ty->getName() == "tuple") {
        return ctx.builder->CreateStructGEP(ctx.getStructTy(root->ty), root->getVariableAddress(ctx), std::stoi(key));
    } else {
        auto s = env->lookup(root->ty->getName());
        if (!s) {
            panic("cannot find root variable, " + root->toString() + "." + key);
        }
        auto field = s->t.findField(key);
        llvm::Value* instance;
        if (root->ty->isStruct()) {
            instance = root->getVariableAddress(ctx);
        } else {
            instance = root->codegen(ctx);
        }
        auto structTy = ctx.getStructTy(root->ty->isPtr() ? root->ty->getElementTy() : root->ty);
        return ctx.builder->CreateStructGEP(structTy, instance, field->index);
    }
}

llvm::Value* Dot::codegen(CompileCtx &ctx, bool enableDeRef) {
    auto address = getVariableAddress(ctx);
    return ctx.builder->CreateLoad(ctx.getTy(ty), address, getName());
}
