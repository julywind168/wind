#include <cstdio>
#include <string>
#include <chrono>

#include "CmdArgs.h"
#include "compiler/Compiler.h"
#include "std/vm.h"


void run(CmdArgs& args) {
    vm_t vm;
    vm_init(&vm);
    vm_loadfile(&vm, args.filename.c_str());

    // profile
    auto start = std::chrono::high_resolution_clock::now();
    int result = vm_eval(&vm);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    if (result != 0)
        printf("Exited with %d\n", result);
    if (elapsed.count() >= 1.0)
        printf("[Finished in %fs]\n", elapsed.count());
    else
        printf("[Finished in %fms]\n", elapsed.count() * 1000.0);
}


int main(int argc, char** argv) {
    auto cmd = CmdArgs(argc, argv);
    try {
        wind::setup();
        if (cmd.mode == Mode::RUN) {
            run(cmd);
        } else {
            auto r = wind::compileFile(cmd.filename, {cmd.optimize, cmd.debug, false});
            if (cmd.mode == Mode::DUMP) {
                printf("%s\n", r.chunk->toString().c_str());
                r.module->print(llvm::errs(), nullptr);
            } else {
                build(std::move(r));
            }
        }
    } catch (const std::exception& e) {
        printf("Exception: %s\n", e.what());
    }
    return 0;
}
