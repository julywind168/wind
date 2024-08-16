#ifndef WIND_S_LOOP_H
#define WIND_S_LOOP_H

#include "ast/expr/Expr.h"


// (loop [init] test body)
class Loop : public Expr {
    std::unique_ptr<Expr> init; // init maybe nullptr
    std::unique_ptr<Expr> test;
    std::unique_ptr<Expr> body;
public:
    Loop(std::unique_ptr<Expr> init, std::unique_ptr<Expr> test, std::unique_ptr<Expr> body)
        : init(std::move(init)), test(std::move(test)), body(std::move(body)) {}

    NodeType nodeTy() const override { return NodeType::LOOP; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx, bool enableDeRef = true) override;

    std::string toString() const override {
        return fmt::format("(loop {}{} {})", init ? init->toString() + " " : "", test->toString(), body->toString());
    }
private:
    std::shared_ptr<Env> myEnv;
};



#endif
