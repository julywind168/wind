#include "ArrayConstr.h"
#include "util/Util.h"


void ArrayConstr::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    std::shared_ptr<wind::Type> ety = nullptr;
    for (auto &e : elements) {
        e->typecheck(env, ety);
        ety = e->ty;
    }
    if (ty->isVoid()) {
        if (elements.empty()) {
            panic("ArrayConstr::typecheck failed, empty array must mark a type (eg: array<i32>)");
        }
        // 'array<Element>
        ty = std::make_shared<wind::NamedType>("array", std::vector<std::shared_ptr<wind::Type>>{ety})->ref();
    } else {
        if (ty->getName() != "array" || ty->getParameters().size() != 1) {
            panic("ArrayConstr::typecheck failed, expected array<T>, got" + ty->toString());
        }
        if (!elements.empty() && !isEqual(ety, ty->getParameters()[0])) {
            panic("ArrayConstr::typecheck failed, expected array<" + ty->getParameters()[0]->toString() + ">, got array<" + ety->toString() + ">");
        }
    }
    initCtor();
    ctor->typecheck(env, ty);
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("StructConstr::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

/**
*   [1, 2, 3] whill be replace by
*   {
*       (let tmp {data: (malloc 8 * (sizeof i32)), size: 0, capacity: 8}''array<i32>)
*       (tmp.append 1)
*       (tmp.append 2)
*       (tmp.append 3)
*       tmp
*   }
*/
void ArrayConstr::initCtor() {
    if (ctor) {
        return;
    }
    std::string code = "";

    auto writeln = [&](std::string s) {
        code += s + "\n";
    };

    auto aty = ty->toString();
    auto ety = ty->getParameters()[0]->toString();
    auto num = nextPowerOf2(elements.size());

    writeln("{");
    writeln(fmt::format("(let tmp {{data: (malloc {} * (sizeof {})), size: 0, capacity: {}}}'{})", num, ety, num, aty));
    for (auto &e : elements) {
        writeln(fmt::format("(tmp.append {})", e->getSourceCode()));
    }
    writeln("tmp");
    writeln("}");

    // printf("ArrayConstr::ctor code:\n%s\n", code.c_str());

    ctor = parseString(env, "__ArrayConstr__", code);
}

llvm::Value* ArrayConstr::codegen(CompileCtx &ctx, bool enableDeRef) {
    if (ctor) {
        return ctor->codegen(ctx);
    } else {
        throw std::runtime_error("ArrayConstr::codegen failed, ctor is null");
    }
}