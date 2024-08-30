#ifndef WIND_DOT_H
#define WIND_DOT_H

#include "Identifier.h"

// p.x
class Dot : public Identifier {
public:
    std::unique_ptr<Identifier> root;
    std::string key;
    Dot(std::unique_ptr<Identifier> root, const std::string& key)
        : root(std::move(root)), key(key) {}

    std::string getName() const { return root->getIdent() + "." + key; }

    NodeType nodeTy() const override { return NodeType::DOT; }

    bool isIdentifier() override { return true; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) override;

    std::shared_ptr<Callee> getCallee(std::vector<std::shared_ptr<wind::Type>> types) override;

    llvm::Value* createCall(CompileCtx &ctx, std::vector<std::shared_ptr<wind::Type>> types, std::vector<llvm::Value*> args) override;

    llvm::Value* getVariableAddress(CompileCtx &ctx) override;

    llvm::Value* codegen(CompileCtx &ctx) override;

    std::string toString() const override { return getName() + ty2Str(); }
private:
    std::shared_ptr<Env> env;
};

#endif
