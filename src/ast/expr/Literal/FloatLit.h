#ifndef WIND_FLOAT_LIT_H
#define WIND_FLOAT_LIT_H

#include "../Expr.h"


class FloatLit : public Expr {
    double value;
public:
    FloatLit(double v) : value(v) {
        ty = wind::Type::F64;
    }

    bool isLiteral() override { return true; }

    NodeType nodeTy() const override { return NodeType::FLOAT_LIT; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override { return std::to_string(value) + ty2Str(); }
private:
    bool marked = false;
};


#endif
