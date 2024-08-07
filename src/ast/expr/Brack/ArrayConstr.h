#ifndef WIND_EXPR_ARRAY_CONSTR_H
#define WIND_EXPR_ARRAY_CONSTR_H

#include "ast/expr/Expr.h"


// [1, 2, 3]
class ArrayConstr : public Expr {
    std::vector<std::unique_ptr<Expr>> elements;
public:
    ArrayConstr(std::vector<std::unique_ptr<Expr>> elements)
        : elements(std::move(elements)) {}

    NodeType nodeTy() const override { return NodeType::ARRAY_CONSTR; }

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return "[...]" + ty2Str();
    }
private:
    std::shared_ptr<Env> env;
    void initCtor();
    std::unique_ptr<Expr> ctor = nullptr;
};



#endif