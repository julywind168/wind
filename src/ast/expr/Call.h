#ifndef WIND_S_H
#define WIND_S_H

#include "Expr.h"


class Call : public Expr {
public:
    std::unique_ptr<Expr> callable;
    std::vector<std::shared_ptr<wind::Type>> generics;
    std::vector<std::unique_ptr<Expr>> params;
    Call(std::unique_ptr<Expr> callable, std::vector<std::shared_ptr<wind::Type>> generics, std::vector<std::unique_ptr<Expr>> params)
        : callable(std::move(callable)), generics(generics), params(std::move(params)) {}

    NodeType nodeTy() const override { return NodeType::CALL; }

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return "(call " + callable->toString() + " ...)";
    }
};


#endif
