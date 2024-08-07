#include "Parser.h"


std::unique_ptr<Expr> Parser::parseBlockExpr(std::shared_ptr<Env> env) {
    if (peek(2)->value == ":") {
        return parseStructConstr(env);
    } else {
        eat('{');
        auto exprs = parseExprList(env);
        eat('}');
        return std::make_unique<Block>(std::move(exprs));
    }
}

// {x: 1, y: 2}
std::unique_ptr<Expr> Parser::parseStructConstr(std::shared_ptr<Env> env) {
    std::vector<std::pair<std::string, std::unique_ptr<Expr>>> fields;
    eat('{');
    while (!curTk->is('}')) {
        auto name = parseIdent();
        eat(':');
        auto expr = parseExpr(env);
        if (!expr) {
            throw std::runtime_error("expected expression after :");
        }
        fields.push_back(std::make_pair(name, std::move(expr)));
        if (!tryEat(",")) {
            break;
        }
    }
    eat('}');
    return std::make_unique<StructConstr>(std::move(fields));
}