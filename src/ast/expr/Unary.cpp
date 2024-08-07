#include "Unary.h"


void Unary::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    if (op == "!") {
        right->typecheck(env, wind::Type::BOOL);
        ty = wind::Type::BOOL;
    } else if (op == "-") {
        right->typecheck(env);
        if (right->ty->isSigned() || right->ty->isFloat()) {
            ty = right->ty;
        } else {
            panic(fmt::format("Unary::typecheck failed, expected signed or float, but {}", right->ty->toString()));
        }
    } else {
        panic(fmt::format("Unary::typecheck failed, unknown operator {}", op));
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Infix::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}


llvm::Value* Unary::codegen(CompileCtx &ctx) {
    auto v = right->codegen(ctx);
    if (op == "!") {
        return ctx.builder->CreateNot(v);
    } else if (op == "-") {
        if (ty->isSigned())
            return ctx.builder->CreateNeg(v, "negtmp");
        else
            return ctx.builder->CreateFNeg(v, "negtmp");
    }
}