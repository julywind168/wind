#include "Call.h"
#include "Identifier.h"
#include "Dot.h"


void Call::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    callable->typecheck(env);
    auto callee = callable->getCallee(generics);

    auto self = callable->nodeTy() == NodeType::DOT ? 1 : 0;
    auto param_size = params.size() + self;

    if (callee->isVarArg) {
        if (param_size < callee->params.size())
            panic("Call: " + callee->name + " minimum takes " + std::to_string(callee->params.size()) + " arguments, but " + std::to_string(param_size) + " were given");
    } else {
        if (param_size != callee->params.size())
            panic("Call: " + callee->name + " takes " + std::to_string(callee->params.size()) + " arguments, but " + std::to_string(param_size) + " were given");
    }
    for (size_t i = 0; i < params.size(); i++) {
        auto &param = params[i];
        auto paramTy = callee->params.size() > i + self ? callee->params[i+self].ty : nullptr;
        param->typecheck(env, paramTy);
        // replace param => (param.__clone)
        if (param->isIdentifier() && env->lookupMeatFunc(param->ty->getName(), "__clone")) {
            params[i] = parseString(env, "__clone", fmt::format("({}.__clone)", param->getSourceCode()));
            params[i]->typecheck(env, paramTy);
        }
    }
    this->ty = callee->retTy->get();
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Call::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Call::codegen(CompileCtx &ctx) {
    std::vector<llvm::Value *> args;
    for (auto &param : params) {
        args.push_back(param->codegen(ctx));
        if (args.back() == nullptr)
            panic("Failed to codegen call argument");
    }
    return callable->createCall(ctx, generics, args);  // createCall maybe change the builder insert point
}
