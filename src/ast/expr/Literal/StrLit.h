#ifndef WIND_STR_LIT_H
#define WIND_STR_LIT_H

#include "../Expr.h"


class StrLit : public Expr {
    std::string value;
public:
    StrLit(std::string v) : value(v) {
        ty = wind::Type::U8->toQuote();
    }

    bool isLiteral() override { return true; }

    NodeType nodeTy() const override { return NodeType::STR_LIT; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override { return value + ty2Str(); }
};



#endif
