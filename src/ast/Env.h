#ifndef WIND_ENV_H
#define WIND_ENV_H

#include <map>
#include <llvm/IR/Instructions.h>
#include "core/Type.h"
#include "fmt/core.h"
#include "expr/I/IFunc.h"

class Expr;
class Callee;
class Protocol_;

enum class SymbolType {
    VARIABLE,
    FUNCTION,
    TYPE,
    PROTOCOL,
    BOX_TYPE        // generic boxed type
};

struct Variable {
    std::shared_ptr<wind::Type> ty;
    bool const_;
    llvm::AllocaInst *address;
};

struct Field {
    size_t index;
    std::string name;
    std::shared_ptr<wind::Type> ty;
};

struct Type_ {
    std::vector<std::pair<std::string, std::shared_ptr<wind::BoxedType>>> generics;
    std::vector<Field> fields;
    std::vector<IFunc*> methods;
    std::vector<std::string> protocols;
public:
    Type_() = default;
    void switchTypes(std::vector<std::shared_ptr<wind::Type>> types);
    Field* findField(std::string key);
    IFunc* findMethod(std::string key);
    bool findProtocol(std::string name);
};

struct Symbol {
    std::string name;
    SymbolType ty;
    Variable v = Variable{nullptr, false, nullptr};
    IFunc* f = nullptr;
    Type_ t = Type_();
    Protocol_* p = nullptr;
    std::shared_ptr<wind::Type> boxtype = nullptr;

public:
    Symbol(std::string name, Variable v) : name(name), ty(SymbolType::VARIABLE), v(v) {}
    Symbol(std::string name, IFunc* f) : name(name), ty(SymbolType::FUNCTION), f(f) {}
    Symbol(std::string name, Type_ t): name(name), ty(SymbolType::TYPE), t(t) {}
    Symbol(std::string name, Protocol_* p): name(name), ty(SymbolType::PROTOCOL), p(p) {}
    Symbol(std::string name, std::shared_ptr<wind::Type> ty) : name(name), ty(SymbolType::BOX_TYPE), boxtype(ty) {}

    Type_* queryType(std::vector<std::shared_ptr<wind::Type>> types) {
        if (ty != SymbolType::TYPE) {
            throw std::runtime_error(fmt::format("symbol {} not a type", name));
        }
        t.switchTypes(types);
        return &t;
    }

    static std::shared_ptr<Symbol> variable_(std::string name, std::shared_ptr<wind::Type> ty, bool const_, llvm::AllocaInst *address) {
        return std::make_shared<Symbol>(name, Variable{ty, const_, address});
    }
    static std::shared_ptr<Symbol> function_(std::string name, IFunc* f) {
        return std::make_shared<Symbol>(name, f);
    }
    static std::shared_ptr<Symbol> type_(std::string name, std::vector<std::pair<std::string, std::shared_ptr<wind::BoxedType>>> generics, std::vector<Field> fields, std::vector<IFunc*> methods, std::vector<std::string> protocols) {
        return std::make_shared<Symbol>(name, Type_{generics, fields, methods, protocols});
    }
    static std::shared_ptr<Symbol> protocol_(std::string name, Protocol_* p) {
        return std::make_shared<Symbol>(name, p);
    }
    static std::shared_ptr<Symbol> boxtype_(std::string name, std::shared_ptr<wind::Type> ty) {
        return std::make_shared<Symbol>(name, ty);
    }
};


class Env: public std::enable_shared_from_this<Env> {

public:
    Env(std::shared_ptr<Env> parent = nullptr) : parent(parent) {}

    void define(std::string name, std::shared_ptr<Symbol> s);
    Symbol* lookup(std::string name);
    IFunc* lookupMeatFunc(std::string tyName, std::string funcName);
    void onClose(CompileCtx &ctx, std::string excepted = "");

private:
    std::map<std::string, std::shared_ptr<Symbol>> symbols;
    std::shared_ptr<Env> parent;
};




#endif
