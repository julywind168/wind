#ifndef WIND_S_ALLOC_H
#define WIND_S_ALLOC_H

#include "ast/expr/Expr.h"


// (alloc type [size])
class Alloc : public Expr {
    std::shared_ptr<wind::Type> basicTy;
    std::unique_ptr<Expr> size;
public:
    Alloc(std::shared_ptr<wind::Type> basicTy, std::unique_ptr<Expr> size): basicTy(basicTy), size(std::move(size)) {}
    NodeType nodeTy() const override { return NodeType::ALLOC; }

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return "(alloc " + basicTy->toString() + (size ? " " + size->toString() : "") + ")";
    }
};



#endif
