#ifndef WIND_S_EXTERN_H
#define WIND_S_EXTERN_H

#include "Callee.h"
#include "ast/expr/Expr.h"
#include "../I/IFunc.h"


class Extern : public Expr, public IFunc {
    std::shared_ptr<Callee> callee;
public:
    Extern(std::shared_ptr<Callee> callee): callee(std::move(callee)) {}

    NodeType nodeTy() const override { return NodeType::EXTERN; }

    std::string getRawName() override {
        return callee->name;
    }

    std::shared_ptr<Callee> getCallee_() override {
        return callee;
    }

    llvm::Function* genfunc(CompileCtx &ctx, std::vector<std::shared_ptr<wind::Type>> types) override {
        if (types.size() != 0) {
            panic("Extern function cannot have generic parameters");
        }
        return callee->codegen(ctx);
    }

    std::string toString() const override {
        return fmt::format("extern {}", callee->toString());
    }
};



#endif