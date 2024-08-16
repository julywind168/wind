#ifndef WIND_S_VAR_H
#define WIND_S_VAR_H

#include "ast/expr/Expr.h"
#include <string>


class Var : public Expr {
    bool const_;
    std::string name;
    std::shared_ptr<wind::Type> hintTy;
    std::unique_ptr<Expr> init;
public:
    Var(bool const_, std::string name, std::shared_ptr<wind::Type> hintTy, std::unique_ptr<Expr> init)
        : const_(const_), name(std::move(name)), hintTy(hintTy), init(std::move(init)) {}

    NodeType nodeTy() const override { return NodeType::VAR; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx, bool enableDeRef = true) override;

    std::string toString() const override {
        return fmt::format("({} {} {})", const_ ? "let": "var", name, init->toString());
    }
private:
    std::shared_ptr<Env> env;
    std::shared_ptr<Symbol> symbol = nullptr;
};



#endif
