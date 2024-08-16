#include "Expr.h"


bool invalidTypeCast(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> from, std::shared_ptr<wind::Type> to) {
    if (from->isVoidPtr() && to->isPtr()) {
        return false;
    }
    if (from->isPtr() && to->isVoidPtr()) {
        return false;
    }
    if (from->isPtr() && !to->isPtr()) {
        if (isEqual(from->getElementTy(), to)) {
            return false;
        }
    }
    auto s = env->lookup(from->toString());
    if (s && s->ty == SymbolType::TYPE && s->t.findProtocol(to->toString())) {
        return false;
    }
    return !isEqual(from, to);
}


bool isEqual(std::shared_ptr<wind::Type> ty, std::shared_ptr<wind::Type> ty2) {
    if (ty == nullptr && ty2 == nullptr) {
        return true;
    }
    if (ty == nullptr || ty2 == nullptr) {
        return false;
    }
    return ty->hash() == ty2->hash();
}

bool isEqual(std::vector<std::shared_ptr<wind::Type>> tyList, std::vector<std::shared_ptr<wind::Type>> tyList2) {
    if (tyList.size() != tyList2.size()) {
        return false;
    }
    for (int i = 0; i < tyList.size(); i++) {
        if (!isEqual(tyList[i], tyList2[i])) {
            return false;
        }
    }
    return true;
}
