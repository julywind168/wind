#ifndef WIND_INFIX_H
#define WIND_INFIX_H

#include "Expr.h"


class Infix : public Expr {
    std::shared_ptr<Expr> left;
    std::string op;
    std::shared_ptr<Expr> right;
public:
    Infix(std::shared_ptr<Expr> l, const std::string& o, std::shared_ptr<Expr> r)
        : left(l), op(o), right(r) {}

    NodeType nodeTy() const override { return NodeType::INFIX; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Instruction::BinaryOps getOpc();

    llvm::CmpInst::Predicate getCmpPred();

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return "(" + left->toString() + " " + op + " " + right->toString() + ")" + ty2Str();
    }
};


#endif
