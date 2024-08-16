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
        // func param expect Ref, disable auto deRef
        paramEnableDeRefs.push_back((paramTy && paramTy->isPtr()) ? false : true);
        param->typecheck(env, paramTy);
    }
    this->ty = callee->retTy->get();
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Call::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::Value* Call::codegen(CompileCtx &ctx, bool enableDeRef) {
    std::vector<llvm::Value *> args;
    for (size_t i = 0; i < params.size(); i++) {
        args.push_back(params[i]->codegen(ctx, paramEnableDeRefs[i]));
        if (args.back() == nullptr)
            panic("Failed to codegen call argument");
    }
    return callable->createCall(ctx, generics, args);  // createCall maybe change the builder insert point
}
