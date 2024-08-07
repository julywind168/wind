#ifndef WIND_EXPR_STRUCT_CONSTR_H
#define WIND_EXPR_STRUCT_CONSTR_H

#include "ast/expr/Expr.h"


// {x: 1, y: 2}
class StructConstr : public Expr {
    std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields;
public:
    StructConstr(std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields)
        : fields(std::move(fields)) {}

    NodeType nodeTy() const override { return NodeType::STRUCT_CONSTR; }

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return "{...}" + ty2Str();
    }
private:
    std::shared_ptr<Env> env;
    Expr* findFieldExprByName(std::string name);
};



#endif
