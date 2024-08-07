#include "Parser.h"


Parser::Parser(std::vector<Token> tokens): tokens(tokens) {
    advance();
}

std::string Parser::advance() {
    if (!curTk || curTk->isEOF()) {
        return "";
    } else {
        auto value = curTk->value;
        for (auto& mt: mtStack) {
            mt.source += (((mt.source != "" && curTk->hasPrecedingSpace) ? " ": "") + value);
        }
        pos = pos + 1;
        curTk = &tokens[pos];
        return value;
    }
}

void Parser::exprBegin() {
    mtStack.push_back(peek()->mt);
}

Meta Parser::exprEnd() {
    auto mt = mtStack.back();
    mtStack.pop_back();
    return mt;
}

Token* Parser::peek(size_t offset) {
    if (pos + offset < tokens.size()) {
        return &tokens[pos + offset];
    }
    return &tokens[tokens.size() - 1];
}

std::unique_ptr<Chunk> Parser::parseChunk() {
    std::shared_ptr<Env> env = std::make_shared<Env>();

    std::vector<std::unique_ptr<Expr>> exprs = parseExprList(env);
    for (auto& e : exprs) {
        if (e->nodeTy() != NodeType::EXTERN
            && e->nodeTy() != NodeType::FUNC
            && e->nodeTy() != NodeType::STRUCT
            && e->nodeTy() != NodeType::PROTOCOL
            && e->nodeTy() != NodeType::IMPL) {
                throw std::runtime_error("Invalid top-level expression: "+ e->toString());
            }
        // printf("  %s\n", e->toString().c_str());
        e->typecheck(env);
    }
    return std::make_unique<Chunk>(env, std::move(exprs));
}

std::unique_ptr<Expr> Parser::parseExpr(std::shared_ptr<Env> env) {
    if (done())
        return nullptr;
    exprBegin();
    auto lhs = parseUnaryExpr(env);
    if (!lhs) {
        exprEnd();
        return nullptr;
    }
    auto e = parseInfixExpr(env, std::move(lhs), 0);
    auto mt = exprEnd();
    if (e) e->mt = mt;
    return e;
}

std::vector<std::unique_ptr<Expr>> Parser::parseExprList(std::shared_ptr<Env> env, char separator) {
    std::vector<std::unique_ptr<Expr>> exprs;
    while (!done()) {
        if (auto e = parseExpr(env)) {
            exprs.push_back(std::move(e));
            if (separator != '\0') {
                if (tryEat(separator)) {
                    // pass
                } else {
                    break;
                }
            }
        } else {
            break;
        }
    }
    return exprs;
}

std::unique_ptr<Chunk> parse(std::vector<Token> tokens) {
    return Parser(tokens).parseChunk();
}

std::unique_ptr<Expr> parseString(std::shared_ptr<Env> env, std::string filename, std::string str) {
    return Parser(tokenize(filename, str)).parseStandaloneExpr(env);
}
