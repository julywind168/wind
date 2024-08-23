#ifndef WIND_S_STORE_H
#define WIND_S_STORE_H

#include "ast/expr/Expr.h"


// (store value to address [offset])
class Store : public Expr {
    std::shared_ptr<Expr> value;
    std::unique_ptr<Expr> address;
    std::unique_ptr<Expr> offset;
public:
    Store(std::unique_ptr<Expr> value, std::unique_ptr<Expr> address, std::unique_ptr<Expr> offset)
        : value(std::move(value)), address(std::move(address)), offset(std::move(offset)) {}
    NodeType nodeTy() const override { return NodeType::STORE; }

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return fmt::format("(store {} to {})", value->toString(), address->toString() + (offset ? " offset " + offset->toString() : ""));
    }
};



#endif
