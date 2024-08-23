#ifndef WIND_PARSER_H
#define WIND_PARSER_H

#include "lexer/Lexer.h"
#include "ast/ast.h"


class Parser {

public:
    Parser(std::vector<Token> tokens);
    ~Parser() = default;

    std::unique_ptr<Chunk> parseChunk();
    std::unique_ptr<Expr> parseStandaloneExpr(std::shared_ptr<Env> env) {
        return parseExpr(env);
    }

private:
    std::vector<Token> tokens;
    size_t pos = -1;
    Token* curTk;

    // top-level
    std::unique_ptr<Expr> parseExpr(std::shared_ptr<Env> env);
    std::vector<std::unique_ptr<Expr>> parseExprList(std::shared_ptr<Env> env, char separator = '\0');

    // atom exprs
    std::unique_ptr<Expr> parseInfixExpr(std::shared_ptr<Env> env, std::unique_ptr<Expr> lhs, int exprPrec);
    std::unique_ptr<Expr> parseUnaryExpr(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parsePrimaryExpr(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseIdentExpr(std::shared_ptr<Env> env);

    // atom literals
    std::unique_ptr<Expr> parseBoolLiteralExpr();
    std::unique_ptr<Expr> parseFltLiteralExpr();
    std::unique_ptr<Expr> parseIntLiteralExpr();
    std::unique_ptr<Expr> parseStrLiteralExpr();

    // atom composites
    std::unique_ptr<Expr> parseSExpr(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseBlockExpr(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseBrackExpr(std::shared_ptr<Env> env);

    // S exprs
    std::unique_ptr<Expr> parseGenericsCall(std::shared_ptr<Env> env);
    std::shared_ptr<Callee> parseCallee(std::shared_ptr<Env> env, std::vector<std::shared_ptr<wind::BoxedType>> structGenerics = {});
    std::unique_ptr<Func> parseFunc(std::shared_ptr<Env> env, bool requireBody, std::vector<std::shared_ptr<wind::BoxedType>> structGenerics = {});
    std::unique_ptr<Expr> parseExtern(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseIf(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseVar(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseLoop(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseCond(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseStruct(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseImpl(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseProtocol(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseSizeof(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseAlloc(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseCast(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseLoad(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseStore(std::shared_ptr<Env> env);
    std::unique_ptr<Expr> parseTupleConstr(std::shared_ptr<Env> env, std::unique_ptr<Expr> first);

    // Block exprs
    std::unique_ptr<Expr> parseStructConstr(std::shared_ptr<Env> env);

    // Brack exprs
    std::unique_ptr<Expr> parseArrayConstr(std::shared_ptr<Env> env);

    // utils
    std::shared_ptr<wind::Type> tryParseMarkedTy(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> defaultTy = nullptr);
    std::shared_ptr<wind::Type> parseTy(std::shared_ptr<Env> env);
    std::string advance();
    Token* peek(size_t offset = 1);

    void parseGenericDecl(std::string structName, std::shared_ptr<Env> env, std::vector<std::pair<std::string, std::shared_ptr<wind::BoxedType>>>& generics);
    std::string parseIdent();
    std::vector<std::string> parseIdentList(char separator);
    std::vector<std::shared_ptr<wind::Type>> parseTyList(std::shared_ptr<Env> env, char separator);
    bool tryEat(std::string str);
    bool tryEat(char c);
    void eat(std::string str);
    void eat(char c);
    bool done();

    // collect expr meta info
    std::vector<Meta> mtStack;
    void exprBegin();
    Meta exprEnd();
};


std::unique_ptr<Chunk> parse(std::vector<Token> tokens);
std::unique_ptr<Expr> parseString(std::shared_ptr<Env> env, std::string filename, std::string str);


#endif