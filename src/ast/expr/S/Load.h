#ifndef WIND_S_LOAD_H
#define WIND_S_LOAD_H

#include "ast/expr/Expr.h"


// (load type from address [offset])
class Load : public Expr {
    std::shared_ptr<wind::Type> valTy;
    std::unique_ptr<Expr> address;
    std::unique_ptr<Expr> offset;
public:
    Load(std::shared_ptr<wind::Type> valTy, std::unique_ptr<Expr> address, std::unique_ptr<Expr> offset)
        : valTy(valTy), address(std::move(address)), offset(std::move(offset)) {}
    NodeType nodeTy() const override { return NodeType::LOAD; }

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return fmt::format("(load {} from {})", valTy->toString(), address->toString() + (offset ? " offset " + offset->toString() : ""));
    }
};



#endif
