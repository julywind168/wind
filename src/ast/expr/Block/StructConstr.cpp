#include "StructConstr.h"
#include "util/Util.h"

void StructConstr::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    this->env = env;
    if (ty->isVoid())
        panic("StructConstr must mark a type");
    auto s = env->lookup(this->ty->getName());
    if (!s || s->ty != SymbolType::TYPE)
        panic("Cannot construct a non-struct type");
    auto t = s->queryType(ty->getParameters());
    for (auto& f : t->fields) {
        auto init = findFieldExprByName(f.name);
        if (!init) {
            panic(fmt::format("StructConstr::typecheck failed, field {} not found", f.name));
        }
        init->typecheck(env, f.ty);
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("StructConstr::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

Expr* StructConstr::findFieldExprByName(std::string name) {
    for (auto& [fieldName, fieldExpr] : fields) {
        if (fieldName == name) {
            return fieldExpr.get();
        }
    }
    return nullptr;
}

llvm::Value* StructConstr::codegen(CompileCtx &ctx) {
    auto s = env->lookup(ty->getName());
    auto t = s->queryType(ty->getParameters());

    llvm::StructType* structTy = ctx.getStructTy(ty->isPtr() ? ty->getElementTy() : ty);
    llvm::Value *instance;
    if (ty->isPtr()) {
        instance = ctx.mallocInstance(structTy, toLowerCase(ty->getName()));
    } else {
        instance = ctx.builder->CreateAlloca(structTy, nullptr, toLowerCase(ty->getName()));
    }
    for (auto& f : t->fields) {
        auto init = findFieldExprByName(f.name);
        auto value = init->codegen(ctx);
        auto address = ctx.builder->CreateStructGEP(structTy, instance, f.index);
        ctx.builder->CreateStore(value, address);
    }
    if (ty->isPtr()) {
        return instance;
    } else {
        return ctx.builder->CreateLoad(structTy, instance, toLowerCase(ty->getName()));
    }
}
