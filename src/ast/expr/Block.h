#ifndef WIND_BLOCK_H
#define WIND_BLOCK_H

#include "Expr.h"


class Block : public Expr {
    std::vector<std::unique_ptr<Expr>> exprs;
public:
    Block(std::vector<std::unique_ptr<Expr>> exprs)
        : exprs(std::move(exprs)) {}

    NodeType nodeTy() const override { return NodeType::BLOCK; }

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx, bool enableDeRef = true) override;

    std::string toString() const override {
        std::string s = "{\n";
        for (const auto& expr : exprs) {
            s += "\t" + expr->toString() + "\n";
        }
        if (s.back() == ' ') {
            s.pop_back();
        }
        s += "  }" + ty2Str();
        return s;
    }
private:
    std::shared_ptr<Env> myEnv;
};


#endif
