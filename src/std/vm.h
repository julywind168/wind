#ifndef WIND_STD_VM_H
#define WIND_STD_VM_H

#include "core/Core.h"
#include "llvm/ExecutionEngine/Orc/IndirectionUtils.h"
#include "llvm/ExecutionEngine/Orc/LLJIT.h"
#include "llvm/ExecutionEngine/Orc/RTDyldObjectLinkingLayer.h"
#include "llvm/ExecutionEngine/Orc/ThreadSafeModule.h"


struct vm_t {
    std::unique_ptr<llvm::orc::LLLazyJIT> jit;
    std::unique_ptr<Chunk> chunk = nullptr;
};


void vm_init(vm_t* vm);
void vm_load(vm_t* vm, const char* code);
void vm_loadfile(vm_t* vm, const char* filename);
int vm_eval(vm_t* vm);

// ret 0, if success
int vm_execute(vm_t* vm, const char* code);



#endif