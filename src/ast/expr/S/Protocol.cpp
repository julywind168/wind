#include "Protocol.h"

void Protocol_::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Protocol can't been mark");
}

std::shared_ptr<Callee> Protocol_::findMethod(std::string name) {
    for (auto &callee : callees) {
        if (callee->name == name) {
            return callee;
        }
    }
    return nullptr;
}

size_t Protocol_::getMethodIndex(std::string name) {
    for (size_t i = 0; i < callees.size(); i++) {
        if (callees[i]->name == name) {
            return i;
        }
    }
    panic("Protocol not found method: " + name);
    return 0;
}

llvm::FunctionType* Protocol_::getMethodFnTy(CompileCtx &ctx, std::string name) {
    for (auto &callee : callees) {
        if (callee->name == name) {
            return callee->getFnTy(ctx);
        }
    }
    panic("Protocol not found method: " + name);
    return nullptr;
}

llvm::StructType* Protocol_::getvTableType(CompileCtx &ctx) {
    auto tyName = "!" + name + "_vTable";
    if (auto ty = llvm::StructType::getTypeByName(*ctx.context, tyName)) {
        return ty;
    }
    std::vector<llvm::Type*> types;
    for (size_t i = 0; i < callees.size(); i++) {
        types.push_back(ctx.getVoidPtrTy());
    }
    return llvm::StructType::create(*ctx.context, types, tyName);
}

// only quote type can follow protocol
llvm::GlobalVariable* Protocol_::getvTable(CompileCtx &ctx, std::shared_ptr<wind::Type> ty) {
    auto vTableName = ty->getName() + "#" + name + "_vTable";
    auto vTable = ctx.module->getNamedGlobal(name);
    if (vTable) {
        return vTable;
    }

    // build vTable
    auto s = env->lookup(ty->toString());
    if (!s || s->ty != SymbolType::TYPE) {
        panic("type " + ty->toString() + " not found");
    }

    auto getFn = [this, &s, &ctx](std::string sign){
        for (auto& m : s->t.methods) {
            if (m->getCallee_()->signatureWithoutSelf() == sign) {
                return m->genfunc(ctx, {});
            }
        }
        panic("method " + sign + " not found");
        return (llvm::Function*)nullptr;
    };

    auto vTableTy = getvTableType(ctx);
    std::vector<llvm::Constant*> vTableMethods;
    for (auto& m : callees) {
        vTableMethods.push_back(getFn(m->signatureWithoutSelf()));
    }
    auto vTableValue = llvm::ConstantStruct::get(vTableTy, vTableMethods);
    return ctx.createGlobalVar(vTableName, vTableValue);
}
