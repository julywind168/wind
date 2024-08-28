#include "CompileCtx.h"

#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Support/TargetSelect.h"

CompileCtx::CompileCtx(CompileOptions options, std::string moduleName) : options(options) {
    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>(moduleName, *context);
    // pass optimization
    theFPM = std::make_unique<llvm::FunctionPassManager>();
    theLAM = std::make_unique<llvm::LoopAnalysisManager>();
    theFAM = std::make_unique<llvm::FunctionAnalysisManager>();
    theCGAM = std::make_unique<llvm::CGSCCAnalysisManager>();
    theMAM = std::make_unique<llvm::ModuleAnalysisManager>();
    thePIC = std::make_unique<llvm::PassInstrumentationCallbacks>();
    theSI = std::make_unique<llvm::StandardInstrumentations>(*context, /*DebugLogging*/ true);
    theSI->registerCallbacks(*thePIC, theMAM.get());
    // Do simple "peephole" optimizations and bit-twiddling optzns.
    theFPM->addPass(llvm::InstCombinePass());
    // Reassociate expressions.
    theFPM->addPass(llvm::ReassociatePass());
    // Eliminate Common SubExpressions.
    theFPM->addPass(llvm::GVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc).
    theFPM->addPass(llvm::SimplifyCFGPass());

    // Register analysis passes used in these transform passes.
    llvm::PassBuilder pb;
    pb.registerModuleAnalyses(*theMAM);
    pb.registerFunctionAnalyses(*theFAM);
    pb.crossRegisterProxies(*theLAM, *theFAM, *theCGAM, *theMAM);

    // init
    setupInternalTypes();
    setupExternFunctions();
}

size_t CompileCtx::getTypeSize(llvm::Type *ty) {
    return module->getDataLayout().getTypeAllocSize(ty);
}

llvm::Align CompileCtx::getStructAlign(llvm::StructType *ty) {
    return module->getDataLayout().getStructLayout(ty)->getAlignment();
};


llvm::Type* CompileCtx::getVoidPtrTy() {
    return llvm::Type::getInt8Ty(*context)->getPointerTo();
}

llvm::Type* CompileCtx::getVoidTy() {
    return llvm::Type::getVoidTy(*context);
}

std::vector<llvm::Type *> CompileCtx::getTyList(std::vector<std::shared_ptr<wind::Type>> types) {
    std::vector<llvm::Type *> ret;
    for (auto& t : types) {
        ret.push_back(getTy(t));
    }
    return ret;
}

llvm::Type* CompileCtx::CompileCtx::getTy(std::shared_ptr<wind::Type> _ty) {
    auto ty = _ty->get();
    if (ty->isPtr()) {
        return llvm::Type::getInt8Ty(*context)->getPointerTo();
    } else if (ty->isVoid()) {
        return llvm::Type::getVoidTy(*context);
    } else if (ty->toString() == "bool") {
        return llvm::Type::getInt1Ty(*context);
    } else if (ty->toString() == "i8" || ty->toString() == "u8") {
        return llvm::Type::getInt8Ty(*context);
    } else if (ty->toString() == "i16" || ty->toString() == "u16") {
        return llvm::Type::getInt16Ty(*context);
    } else if (ty->toString() == "i32" || ty->toString() == "u32") {
        return llvm::Type::getInt32Ty(*context);
    } else if (ty->toString() == "i64" || ty->toString() == "u64") {
        return llvm::Type::getInt64Ty(*context);
    } else if (ty->toString() =="f32") {
        return llvm::Type::getFloatTy(*context);
    } else if (ty->toString() == "f64") {
        return llvm::Type::getDoubleTy(*context);
    } else {
        if (ty->isProtocol()) {
            return structTyCache["!protocol"];
        } else if (ty->isStruct()) {
            return getStructTy(ty);
        } else {
            throw std::runtime_error("Unknown type: " + ty->toString());
        }
    }
}

// if ty is a generic type, you must switch parameters of type before calling this function
llvm::StructType* CompileCtx::getStructTy(std::shared_ptr<wind::Type> _ty) {
    auto ty = _ty->get();
    auto name = ty->hash();
    if (structTyCache.find(name) != structTyCache.end()) {
        return structTyCache[name];
    } else {
        auto fields = ty->getName() == "tuple" ? ty->getParameters() : wind::NamedType::getStructFieldTys(ty->getName());
        std::vector<llvm::Type *> types;
        for (auto t : fields) {
            types.push_back(getTy(t));
        }
        auto ty = llvm::StructType::create(*context, types, name);
        structTyCache[name] = ty;
        return ty;
    }
}

llvm::StructType* CompileCtx::getProtocolStructTy() {
    return structTyCache["!protocol"];
}

llvm::GlobalVariable* CompileCtx::createGlobalVar(const std::string& name, llvm::Constant* init) {
    module->getOrInsertGlobal(name, init->getType());
    auto variable = module->getNamedGlobal(name);
    variable->setAlignment(llvm::MaybeAlign(4));
    variable->setConstant(false);
    variable->setInitializer(init);
    return variable;
}

void CompileCtx::setupInternalTypes() {
    wind::NamedType::defineStruct("tuple", {});
    structTyCache["!protocol"] = llvm::StructType::create(*context, {getVoidPtrTy(), getVoidPtrTy()}, "!protocol");
}

void CompileCtx::setupExternFunctions() {
    // pass
}
