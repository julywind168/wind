#include "StrLit.h"
#include <regex>

void StrLit::markTy(std::shared_ptr<wind::Type> ty) {
    panic("StrLit has default `ptr` type, can't been mark");
}

std::string trim_first_last_char(std::string str) {
    if (str.length() > 1) {
        str = str.substr(1, str.length() - 2);
    } else if (str.length() == 1) {
        str.clear();
    }
    return str;
}

std::string realString(std::string s) {
    s = trim_first_last_char(s); // discard `"`
    auto re = std::regex("\\\\n");
    return std::regex_replace(s, re, "\n");
}

llvm::Value* StrLit::codegen(CompileCtx &ctx) {
    llvm::Constant *strConstant = llvm::ConstantDataArray::getString(*ctx.context, realString(value), /* add null */true);
    llvm::GlobalVariable *strGlobal = new llvm::GlobalVariable(
        *ctx.module,
        strConstant->getType(),
        true,
        llvm::GlobalValue::PrivateLinkage,
        strConstant
    );
    return strGlobal;
}
