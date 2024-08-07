#include "Callee.h"

void Callee::markSelfTy(std::shared_ptr<wind::Type> ty) {
    if (params.size() == 0 || params[0].name != "self") {
        throw std::runtime_error("Callee::markSelfTy: self param not found");
    }
    params[0].ty = ty;
    prefix = ty->getName() + "/";
}

void Callee::switchTypes(std::vector<std::shared_ptr<wind::Type>> types) {
    if (generics.size() != types.size()) {
        throw std::runtime_error("Callee::switchTypes: generics size mismatch");
    }
    if (!generics.empty()) {
        for (int i = 0; i < generics.size(); i++) {
            generics[i]->set(types[i]);
        }
        // fill self's type
        if (params.size() > 0 && params[0].name == "self") {
            params[0].ty->setParameters(types);
        }
    }
}

std::string Callee::fullName() {
    auto fname = prefix + name;
    for (auto& t : generics) {
        fname += "#" + t->hash();
    }
    return fname;
}

std::string Callee::signature() {
    auto s = name;
    for (auto& p : params) {
        s += "/" + p.ty->toString();
    }
    return s;
}

std::string Callee::signatureWithoutSelf() {
    auto s = name;
    for (auto& p : params) {
        if (p.name != "self") {
            s += "/" + p.ty->toString();
        }
    }
    return s;
}

std::string paramsToString(const std::vector<Param>& params) {
    std::string result = "";
    for (auto& param : params) {
        result += param.toString() + " ";
    }
    if (!result.empty())
        result.pop_back();
    return result;
}

std::string Callee::paramsStr() {
    std::string s = "";
    if (params.size() > 0 || isVarArg)
        s += " ";
    s += paramsToString(params);
    if (isVarArg)
        s += " ...";
    return s;
}

std::vector<llvm::Type *> Callee::getParamTys(CompileCtx &ctx) {
    std::vector<llvm::Type *> paramTys;
    for (auto &param : params) {
        paramTys.push_back(ctx.getTy(param.ty));
    }
    return paramTys;
}

llvm::FunctionType* Callee::getFnTy(CompileCtx &ctx) {
    return llvm::FunctionType::get(ctx.getTy(retTy), getParamTys(ctx), isVarArg);
}

llvm::Function* Callee::codegen(CompileCtx &ctx) {
    if (auto f = ctx.module->getFunction(fullName())) {
        return f;
    }
    return llvm::Function::Create(getFnTy(ctx), llvm::Function::ExternalLinkage, fullName(), *ctx.module);
}