#ifndef WIND_S_SIZEOF_H
#define WIND_S_SIZEOF_H

#include "ast/expr/Expr.h"


class Sizeof : public Expr {
    std::shared_ptr<wind::Type> type;
public:
    Sizeof(std::shared_ptr<wind::Type> type) : type(type) {}
    NodeType nodeTy() const override { return NodeType::SIZEOF; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return "(sizeof " + type->toString() + ")";
    }
};



#endif
