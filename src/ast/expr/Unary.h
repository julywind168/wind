#ifndef WIND_UNARY_H
#define WIND_UNARY_H

#include "Expr.h"


// - !
class Unary : public Expr {
    std::string op;
    std::shared_ptr<Expr> right;
public:
    Unary(const std::string& o, std::shared_ptr<Expr> e)
        : op(o), right(e) {}

    NodeType nodeTy() const override { return NodeType::UNARY; }

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return "(" + op + right->toString() + ")" + ty2Str();
    }
};


#endif
