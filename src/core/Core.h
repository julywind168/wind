#ifndef WIND_CORE_H
#define WIND_CORE_H

#include <string>
#include "ast/ast.h"

struct CompileResult
{
    std::unique_ptr<Chunk> chunk;
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
};

namespace wind {
    void setup();
    CompileResult compileString(std::string code, CompileOptions options = CompileOptions());
    CompileResult compileFile(std::string filename, CompileOptions options = CompileOptions());
    CompileResult compileProject(std::string dir, CompileOptions options = CompileOptions());
};


#endif
