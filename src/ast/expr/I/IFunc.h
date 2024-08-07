#ifndef WIND_I_IFUNC_H
#define WIND_I_IFUNC_H

#include <string>
#include "core/Type.h"
#include "llvm/IR/LLVMContext.h"


class Callee;
class CompileCtx;


class IFunc {

public:
    virtual std::string getRawName() = 0;
    virtual std::shared_ptr<Callee> getCallee_() = 0;
    virtual llvm::Function* genfunc(CompileCtx &ctx, std::vector<std::shared_ptr<wind::Type>> types) = 0;
};


#endif
