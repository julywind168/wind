#ifndef WIND_S_CALEE_H
#define WIND_S_CALEE_H

#include <string>
#include <fmt/core.h>
#include "core/Type.h"
#include "core/CompileCtx.h"

struct Param {
    std::string name;
    std::shared_ptr<wind::Type> ty;
    std::string toString() const {
        return name + "\'" + ty->toString();
    }
};

std::string paramsToString(const std::vector<Param>& params);


class Callee {
public:
    std::string name;
    std::vector<std::shared_ptr<wind::BoxedType>> generics;
    std::vector<Param> params;
    std::shared_ptr<wind::Type> retTy;
    bool isVarArg;

    std::string prefix = "";

    Callee(std::string name, std::vector<std::shared_ptr<wind::BoxedType>> generics, std::vector<Param> params, std::shared_ptr<wind::Type> retTy, bool isVarArg)
        : name(std::move(name)), generics(generics), params(std::move(params)), retTy(std::move(retTy)), isVarArg(isVarArg){}

    void markSelfTy(std::shared_ptr<wind::Type> ty);

    void switchTypes(std::vector<std::shared_ptr<wind::Type>> types);
    std::string fullName();

    std::string signature();
    std::string signatureWithoutSelf();
    
    std::vector<llvm::Type*> getParamTys(CompileCtx &ctx);

    llvm::FunctionType* getFnTy(CompileCtx &ctx);

    llvm::Function* codegen(CompileCtx &ctx);

    std::string paramsStr();
    std::string toString() {
        return fmt::format("({}{})'{}", name, paramsStr(), retTy->toString());
    }
};


#endif