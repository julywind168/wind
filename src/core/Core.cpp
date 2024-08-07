#include "Core.h"
#include "util/Util.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"

#include "llvm/Support/TargetSelect.h"

namespace wind {

void setup() {
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllDisassemblers();
}

CompileResult _compile(std::vector<Token>& tokens, CompileOptions options) {
    auto chunk = parse(tokens);
    auto ctx = CompileCtx(options, "main");
    chunk->codegen(ctx);
    return {std::move(chunk), std::move(ctx.context), std::move(ctx.module)};
}

CompileResult compileString(std::string code, CompileOptions options) {
    auto tokens = tokenize("anonymous", code);
    return _compile(tokens, options);
}

CompileResult compileFile(std::string filename, CompileOptions options) {
    auto code = readFile(filename);
    auto tokens = tokenize(filename, code);
    return _compile(tokens, options);
}
}
