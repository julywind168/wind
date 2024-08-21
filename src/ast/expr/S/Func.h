#ifndef WIND_S_FUNC_H
#define WIND_S_FUNC_H

#include "Callee.h"
#include "ast/expr/Expr.h"
#include "../I/IFunc.h"


struct FuncInstance{
    std::shared_ptr<Env> env;
    std::vector<std::shared_ptr<wind::Type>> types;
    llvm::Function* f;
};

class Func : public Expr, public IFunc {
    std::shared_ptr<Env> myEnv;
    std::vector<FuncInstance> instances;
public:
    std::shared_ptr<Callee> callee;
    std::unique_ptr<Expr> body;
    Func(std::shared_ptr<Env> myEnv, std::shared_ptr<Callee> callee, std::unique_ptr<Expr> body)
        : myEnv(myEnv), callee(std::move(callee)), body(std::move(body))
    {}
    NodeType nodeTy() const override { return NodeType::FUNC; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    std::string getRawName() override {
        return callee->name;
    }

    std::shared_ptr<Callee> getCallee_() override {
        return callee;
    }

    llvm::Function* findInstance(std::vector<std::shared_ptr<wind::Type>> types);

    llvm::Function* genfunc(CompileCtx &ctx, std::vector<std::shared_ptr<wind::Type>> types) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return fmt::format("func {} -> {}", callee->toString(), body->toString());
    }
};



#endif
