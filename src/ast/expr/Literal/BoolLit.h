#ifndef WIND_BOOL_LIT_H
#define WIND_BOOL_LIT_H

#include "../Expr.h"


class BoolLit : public Expr {
    bool value;
public:
    BoolLit(bool v) : value(v) {
        ty = wind::Type::BOOL;
    }

    NodeType nodeTy() const override { return NodeType::BOOL_LIT; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override { return (value ? "true" : "false"); }
};


#endif
