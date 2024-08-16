#include "Cast.h"

void Cast::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    source->typecheck(env);
    if (!source->ty->isInteger() && !source->ty->isFloat()) {
        panic(fmt::format("Cast::typecheck failed, expected number, but source: {}", source->ty->toString()));
    }
    if (!dstTy->isInteger() && !dstTy->isFloat()) {
        panic(fmt::format("Cast::typecheck failed, expected number, but dstTy: {}", dstTy->toString()));
    }
    ty = dstTy;
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Cast::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Cast::codegen(CompileCtx &ctx, bool enableDeRef) {
    auto srcTy = source->ty;
    auto val = source->codegen(ctx);
    if (dstTy->isInteger() && srcTy->isFloat()) {
        if (dstTy->isUnsigned()) {
            return ctx.builder->CreateFPToUI(val, ctx.getTy(dstTy));
        } else {
            return ctx.builder->CreateFPToSI(val, ctx.getTy(dstTy));
        }
    }
    if (dstTy->isFloat() && srcTy->isInteger()) {
        if (srcTy->isUnsigned()) {
            return ctx.builder->CreateUIToFP(val, ctx.getTy(dstTy));
        } else {
            return ctx.builder->CreateSIToFP(val, ctx.getTy(dstTy));
        }
    }

    if (srcTy->isInteger() && dstTy->isInteger()) {
        if (ctx.getTypeSize(ctx.getTy(dstTy)) > ctx.getTypeSize(ctx.getTy(srcTy))) {
            if (srcTy->isSigned()) {
                return ctx.builder->CreateSExt(val, ctx.getTy(dstTy));
            } else {
                return ctx.builder->CreateZExt(val, ctx.getTy(dstTy));
            }
        } else {
            return ctx.builder->CreateTrunc(val, ctx.getTy(dstTy));
        }
    } else {
        return ctx.builder->CreateFPCast(val, ctx.getTy(dstTy));
    }
}
