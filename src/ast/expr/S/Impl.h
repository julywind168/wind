#ifndef WIND_S_IMPL_H
#define WIND_S_IMPL_H

#include "ast/expr/Expr.h"
#include "Func.h"


class Impl : public Expr {
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<wind::BoxedType>>> generics;
    std::vector<std::string> protocols;
    std::vector<std::unique_ptr<Func>> funcs;
public:
    Impl(std::string name, std::vector<std::pair<std::string, std::shared_ptr<wind::BoxedType>>> generics, std::vector<std::string> protocols, std::vector<std::unique_ptr<Func>> funcs)
        : name(name), generics(generics), protocols(protocols), funcs(std::move(funcs)) {}

    NodeType nodeTy() const override { return NodeType::IMPL; }

    bool isGeneric() { return !generics.empty(); }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override {
        return fmt::format("(impl {} ...)", name);
    }
};



#endif
