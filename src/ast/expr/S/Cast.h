#ifndef WIND_S_CAST_H
#define WIND_S_CAST_H

#include "ast/expr/Expr.h"


class Cast : public Expr {
    std::shared_ptr<wind::Type> dstTy;
    std::unique_ptr<Expr> source;
public:
    Cast(std::shared_ptr<wind::Type> dstTy, std::unique_ptr<Expr> source)
        : dstTy(dstTy), source(std::move(source)) {}
    NodeType nodeTy() const override { return NodeType::CAST; }

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx, bool enableDeRef = true) override;

    std::string toString() const override {
        return "(cast " + dstTy->toString() + " source: " + source->toString() + ")";
    }
};



#endif
