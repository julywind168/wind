#include "Env.h"
#include "ast/expr/S/Callee.h"

void Type_::switchTypes(std::vector<std::shared_ptr<wind::Type>> types) {
    if (types.size() != generics.size()) {
        throw std::runtime_error(fmt::format("Type_::switchTypes: types.size {} != generics.size {}", types.size(), generics.size()));
    }
    for (size_t i = 0; i < generics.size(); i++) {
        generics[i].second->set(types[i]);
    }
}

Field* Type_::findField(std::string key) {
    for (auto& field : fields) {
        if (field.name == key) {
            return &field;
        }
    }
    return nullptr;
}
IFunc* Type_::findMethod(std::string key) {
    for (auto& method : methods) {
        if (method->getRawName() == key) {
            return method;
        }
    }
    return nullptr;
}

bool Type_::findProtocol(std::string name) {
    for (auto& protocol : protocols) {
        if (protocol == name) {
            return true;
        }
    }
    return false;
}

void Env::define(std::string name, std::shared_ptr<Symbol> s) {
    if (symbols.find(name) != symbols.end()) {
        throw std::runtime_error("Redefinition of " + name);
    }
    symbols[name] = s;
}

Symbol* Env::lookup(std::string name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.get();
    }
    if (parent) {
        return parent->lookup(name);
    }
    return nullptr;
}