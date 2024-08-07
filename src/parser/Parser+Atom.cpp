#include "Parser.h"

std::unique_ptr<Expr> Parser::parseInfixExpr(std::shared_ptr<Env> env, std::unique_ptr<Expr> lhs, int exprPrec) {
    while (true) {
        int tokPrec = curTk->precedence();
        if (tokPrec < exprPrec)
            return lhs;
        auto op = advance();
        exprBegin();
        auto rhs = parseUnaryExpr(env);
        if (!rhs)
            return nullptr;
        int nextPrec = curTk->precedence();
        if (tokPrec < nextPrec) {
            rhs = parseInfixExpr(env, std::move(rhs), tokPrec + 1);
            if (rhs) {
                rhs->mt = exprEnd();
            } else {
                exprEnd();
                return nullptr;
            }
        } else {
            exprEnd();
        }
        if (op == "=")
            lhs = std::make_unique<Equal>(std::move(lhs), std::move(rhs));
        else
            lhs = std::make_unique<Infix>(std::move(lhs), std::move(op), std::move(rhs));
    }
}

// - !
std::unique_ptr<Expr> Parser::parseUnaryExpr(std::shared_ptr<Env> env) {
    std::unique_ptr<Expr> expr = nullptr;
    exprBegin();
    if (curTk->is('-') || curTk->is('!')) {
        if (curTk->hasFollowingSpace) {
            throw std::runtime_error("space is not allowed after unary operator: " + curTk->toString());
        }
        auto op = advance();
        auto right = parseUnaryExpr(env);
        if (!right) {
            throw std::runtime_error("unexpected token: " + curTk->toString());
        }
        expr = std::make_unique<Unary>(op, std::move(right));
    } else {
        expr = parsePrimaryExpr(env);
    }
    auto mt = exprEnd();
    if (expr) {
        expr->mt = mt;
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parsePrimaryExpr(std::shared_ptr<Env> env) {
    std::unique_ptr<Expr> expr = nullptr;
    exprBegin();
    if (curTk->type == TokenType::IDENT || curTk->type == TokenType::RESERVED) {
        expr = parseIdentExpr(env);
    } else if (curTk->type == TokenType::STRING) {
        expr = parseStrLiteralExpr();
    } else if (curTk->type == TokenType::INT) {
        expr = parseIntLiteralExpr();
    } else if (curTk->type == TokenType::FLOAT) {
        expr = parseFltLiteralExpr();
    } else if (curTk->type == TokenType::BOOL) {
        expr = parseBoolLiteralExpr();
    } else if (curTk->type == TokenType::L_PAREN) {
        expr = parseSExpr(env);
    } else if (curTk->type == TokenType::L_BRACE) {
        expr = parseBlockExpr(env);
    } else if (curTk->type == TokenType::L_BRACK) {
        expr = parseBrackExpr(env);
    }
    if (expr) {
        if (auto ty = tryParseMarkedTy(env)) {
            expr->markTy(ty);
        }
        expr->mt = exprEnd();
    } else {
        exprEnd();
    }
    return expr;
}

std::unique_ptr<Expr> Parser::parseStrLiteralExpr() {
    auto value = advance();
    return std::make_unique<StrLit>(value);
}

std::unique_ptr<Expr> Parser::parseBoolLiteralExpr() {
    auto value = advance();
    return std::make_unique<BoolLit>(value == "true");
}

std::unique_ptr<Expr> Parser::parseFltLiteralExpr() {
    auto value = advance();
    return std::make_unique<FloatLit>(std::stod(value));
}

std::unique_ptr<Expr> Parser::parseIntLiteralExpr() {
    auto value = advance();
    return std::make_unique<IntLit>(static_cast<uint64_t>(std::stoll(value, 0, 10)));
}

std::unique_ptr<Expr> Parser::parseIdentExpr(std::shared_ptr<Env> env) {
    exprBegin();
    auto name = advance();
    auto root = std::make_unique<Identifier>(name);
    root->mt = exprEnd();

    while (true) {
        if (curTk->is(".")) {
            advance();
            if (curTk->type != TokenType::IDENT && curTk->type != TokenType::INT)
                throw std::runtime_error("Expected identifier or interger after '.'");
            auto key = advance();
            root = std::make_unique<Dot>(std::move(root), key);
        } else if (curTk->is("[")) {
            advance();
            auto key = parseExpr(env);
            eat("]");
            root = std::make_unique<Index>(std::move(root), std::move(key));
        } else {
            break;
        }
    }
    return root;
}
