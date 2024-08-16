#ifndef WIND_CHUNK_H
#define WIND_CHUNK_H

#include "Expr.h"


class Chunk : public Expr {
    std::shared_ptr<Env> env;
    std::vector<std::unique_ptr<Expr>> exprs;
public:
    Chunk(std::shared_ptr<Env> env, std::vector<std::unique_ptr<Expr>> exprs) : env(env), exprs(std::move(exprs)) {}
    
    NodeType nodeTy() const override { return NodeType::CHUNK; }

    llvm::Value* codegen(CompileCtx &ctx, bool enableDeRef = true) override {
        for (auto& expr : exprs) {
            expr->codegen(ctx);
        }
        return nullptr;
    }
    
    std::string toString() const override {
        std::string s = "chunk:\n";
        for (const auto& expr : exprs) {
            s += "  " + expr->toString() + "\n";
        }
        return s;
    }
};


#endif