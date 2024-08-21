#ifndef WIND_INT_LIT_H
#define WIND_INT_LIT_H

#include "../Expr.h"


class IntLit : public Expr {
public:
    uint64_t value;
    IntLit(uint64_t v) : value(v) {
        ty = wind::Type::I32;
    }

    bool isLiteral() override { return true; }

    NodeType nodeTy() const override { return NodeType::INT_LIT; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override { return std::to_string(value) + ty2Str(); }
private:
    bool marked = false;
};



#endif
