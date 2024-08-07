#include "Compiler.h"

#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/IR/LegacyPassManager.h"


void build(CompileResult r, std::string output) {
    auto targetTriple = llvm::sys::getDefaultTargetTriple();
    r.module->setTargetTriple(targetTriple);
    std::string error;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
        throw std::runtime_error(error);
    }

    auto cpu = "generic";
    auto features = "";

    llvm::TargetOptions opt;
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, llvm::Reloc::PIC_);
    r.module->setDataLayout(targetMachine->createDataLayout());

    auto filename = output + ".o";
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    if (ec) {
        throw std::runtime_error("Could not open file: " + ec.message());
    }

    llvm::legacy::PassManager pass;
    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, llvm::CodeGenFileType::ObjectFile)) {
        throw std::runtime_error("targetMachine can't emit a file of this type");
    }

    pass.run(*r.module);
    dest.flush();

    std::string cmd = fmt::format("clang++ -O2 -g -o {} {} -I/opt/homebrew/opt/libgc/include /opt/homebrew/opt/libgc/lib/libgc.dylib src/runtime/runtime.cpp", output, filename);
    int err = system(cmd.c_str());
    if (err) {
        throw std::runtime_error("failed to run linker");
    } else {
        printf("Build `%s` success\n", output.c_str());
    }
}
