#ifndef WIND_INDEX_H
#define WIND_INDEX_H

#include "Identifier.h"

// a[0]
class Index : public Identifier {
public:
    std::unique_ptr<Identifier> root;
    std::unique_ptr<Expr> key;
    Index(std::unique_ptr<Identifier> root, std::unique_ptr<Expr> key)
        : root(std::move(root)), key(std::move(key)) {}

    NodeType nodeTy() const override { return NodeType::INDEX; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override { return root->getIdent() + "[" + key->toString() + "]" + ty2Str(); }

private:
    std::shared_ptr<Env> env;
    std::unique_ptr<Expr> createIndexCall();
};

#endif
