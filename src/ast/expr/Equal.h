#ifndef WIND_EQUAL_H
#define WIND_EQUAL_H

#include "Expr.h"


class Equal : public Expr {
    std::shared_ptr<Expr> left;
    std::shared_ptr<Expr> right;
public:
    Equal(std::shared_ptr<Expr> left, std::shared_ptr<Expr> right)
        : left(std::move(left)), right(std::move(right)) {}

    NodeType nodeTy() const override { return NodeType::EQUAL; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx, bool enableDeRef = true) override;

    std::string toString() const override {
        return fmt::format("({} = {})", left->toString(), right->toString());
    }

private:
    std::shared_ptr<Env> env;
    std::unique_ptr<Expr> createNewIndexCall();
};



#endif
