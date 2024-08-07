#ifndef WIND_IDENTIFIER_H
#define WIND_IDENTIFIER_H

#include "Expr.h"


class Identifier : public Expr {
    std::string name;
public:
    Identifier(){}
    Identifier(const std::string& n) : name(n) {}

    std::string getIdent() const { return name; }

    Symbol* getSymbol() { return symbol; }

    NodeType nodeTy() const override { return NodeType::IDENTIFIER; }

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    std::shared_ptr<Callee> getCallee(std::vector<std::shared_ptr<wind::Type>> types) override;

    llvm::Value* createCall(CompileCtx &ctx, std::vector<std::shared_ptr<wind::Type>> types, std::vector<llvm::Value*> args) override;

    llvm::Value* getVariableAddress(CompileCtx &ctx) override {
        return (symbol && symbol->ty == SymbolType::VARIABLE) ? symbol->v.address: nullptr;
    }

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override { return name + ty2Str(); }
private:
    std::shared_ptr<Env> env;
    Symbol* symbol = nullptr;
};



#endif
