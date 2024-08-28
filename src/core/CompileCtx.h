#ifndef WIND_COMPILE_CTX_H
#define WIND_COMPILE_CTX_H

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"
#include "llvm/IR/Verifier.h"

#include "Type.h"
#include "ast/Env.h"

struct CompileOptions {
    bool optimize = false;
    bool debug = false;
    bool script = false;
};

class CompileCtx {
public:
    CompileOptions options;

    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    // pass
    std::unique_ptr<llvm::FunctionPassManager> theFPM;
    std::unique_ptr<llvm::LoopAnalysisManager> theLAM;
    std::unique_ptr<llvm::FunctionAnalysisManager> theFAM;
    std::unique_ptr<llvm::CGSCCAnalysisManager> theCGAM;
    std::unique_ptr<llvm::ModuleAnalysisManager> theMAM;
    std::unique_ptr<llvm::PassInstrumentationCallbacks> thePIC;
    std::unique_ptr<llvm::StandardInstrumentations> theSI;

    CompileCtx(CompileOptions options, std::string moduleName);

    llvm::Type* getVoidPtrTy();
    llvm::Type* getVoidTy();
    llvm::Type* getTy(std::shared_ptr<wind::Type> ty);
    std::vector<llvm::Type *> getTyList(std::vector<std::shared_ptr<wind::Type>> types);
    size_t getTypeSize(llvm::Type *ty);
    llvm::Align getStructAlign(llvm::StructType *ty);
    llvm::StructType* getStructTy(std::shared_ptr<wind::Type> _ty);
    llvm::StructType* getProtocolStructTy();
    llvm::GlobalVariable* createGlobalVar(const std::string& name, llvm::Constant* init);
    
private:
    void setupExternFunctions();
    void setupInternalTypes();
    std::map<std::string, llvm::StructType*> structTyCache;
};




#endif
