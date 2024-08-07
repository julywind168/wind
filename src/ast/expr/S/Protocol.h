#ifndef WIND_S_PROTOCOL_H
#define WIND_S_PROTOCOL_H

#include "ast/expr/Expr.h"


class Protocol_ : public Expr {
    std::shared_ptr<Env> env;
    std::string name;
    std::vector<std::shared_ptr<Callee>> callees;
public:
    Protocol_(std::shared_ptr<Env> env, std::string name, std::vector<std::shared_ptr<Callee>> callees)
        : env(env), name(std::move(name)), callees(std::move(callees)) {}

    NodeType nodeTy() const override { return NodeType::PROTOCOL; }

    void markTy(std::shared_ptr<wind::Type> ty) override;
    
    std::shared_ptr<Callee> findMethod(std::string name);

    size_t getMethodIndex(std::string name);

    llvm::FunctionType* getMethodFnTy(CompileCtx &ctx, std::string name);

    llvm::StructType* getvTableType(CompileCtx &ctx);

    llvm::GlobalVariable* getvTable(CompileCtx &ctx, std::shared_ptr<wind::Type> ty);

    std::string toString() const override {
        return fmt::format("(protocol {} ...)", name);
    }
};



#endif
