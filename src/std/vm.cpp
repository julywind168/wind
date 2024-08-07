#include "vm.h"

static llvm::ExitOnError ExitOnErr;

void vm_init(vm_t* vm) {
    vm->jit = ExitOnErr(llvm::orc::LLLazyJITBuilder().create());
}


void vm_load(vm_t* vm, const char* code) {

}

void vm_loadfile(vm_t* vm, const char* filename) {
    auto r = wind::compileFile(filename, {false, false, true});
    vm->chunk = std::move(r.chunk);
    ExitOnErr(vm->jit->addIRModule(llvm::orc::ThreadSafeModule(std::move(r.module), std::move(r.context))));
}

// eval the main
int vm_eval(vm_t* vm) {
    if (vm->chunk == nullptr) {
        fprintf(stderr, "vm has not loaded any code yet");
        return -1;
    }
    if (auto mainAddr = vm->jit->lookup("main")) {
        int (*main)() = (int(*)())mainAddr.get().getValue();
        return main();
    } else {
        fprintf(stderr, "vm can't found main function(eg: `(func (main)'i32 { 0 })`)");
        return -1;
    }
}

// ret 0, if success
int vm_execute(vm_t* vm, const char* code) {
    return 0;
}
