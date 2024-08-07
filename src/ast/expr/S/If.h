#ifndef WIND_S_IF_H
#define WIND_S_IF_H

#include "ast/expr/Expr.h"


class If : public Expr {
    std::unique_ptr<Expr> _test;
    std::unique_ptr<Expr> _then;
    std::unique_ptr<Expr> _else;
public:
    If(std::unique_ptr<Expr> test, std::unique_ptr<Expr> then, std::unique_ptr<Expr> els)
        : _test(std::move(test)), _then(std::move(then)), _else(std::move(els))
    {}

    NodeType nodeTy() const override { return NodeType::IF; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return fmt::format("(if {} {} {})", _test->toString(), _then->toString(), _else->toString());
    }
private:
    std::shared_ptr<Env> thenEnv;
    std::shared_ptr<Env> elseEnv;
};



#endif
