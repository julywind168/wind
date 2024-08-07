#include "Parser.h"


std::unique_ptr<Expr> Parser::parseBrackExpr(std::shared_ptr<Env> env) {
    return parseArrayConstr(env);
}

// [1, 2, 3]
std::unique_ptr<Expr> Parser::parseArrayConstr(std::shared_ptr<Env> env) {
    eat('[');
    auto elements = parseExprList(env, ',');
    eat(']');
    return std::make_unique<ArrayConstr>(std::move(elements));
}