#ifndef WIND_S_STRUCT_H
#define WIND_S_STRUCT_H

#include "ast/expr/Expr.h"
#include <string>


class Struct : public Expr {
    std::shared_ptr<Env> myEnv;
    std::string name;
    std::vector<std::pair<std::string, std::shared_ptr<wind::BoxedType>>> generics;
    std::vector<Field> fields;
public:
    Struct(std::shared_ptr<Env> myEnv, std::string name, std::vector<std::pair<std::string, std::shared_ptr<wind::BoxedType>>> generics, std::vector<Field> fields)
        : myEnv(myEnv), name(name), generics(generics), fields(fields) {}

    NodeType nodeTy() const override { return NodeType::STRUCT; }

    void markTy(std::shared_ptr<wind::Type> ty) override;

    std::string toString() const override {
        return fmt::format("(struct {} ...)", name);
    }
private:
    bool isGeneric() { return !generics.empty(); }
};



#endif
