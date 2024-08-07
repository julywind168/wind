#ifndef WIND_S_COND_H
#define WIND_S_COND_H

#include "ast/expr/Expr.h"
#include <vector>


class Cond : public Expr {
    std::vector<std::shared_ptr<Env>> caseEnvs;
    std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> cases;
public:
    Cond(std::vector<std::shared_ptr<Env>> caseEnvs, std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> cases)
        : caseEnvs(caseEnvs), cases(std::move(cases)) {}

    NodeType nodeTy() const override { return NodeType::COND; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return "(cond ...)";
    }
};



#endif
