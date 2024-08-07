#include "Token.h"
#include <set>
#include <fmt/format.h>


static std::set<std::string> Reserved = {
    "true", "false", "nil",
    "var", "let",
    "struct", "impl", "protocol", "extern", "func", "if", "loop", "cond", "match",
    // pre-reserved
    "for", "in"
};

bool isReserved(std::string text) {
    return Reserved.count(text) > 0;
}

Token::Token(TokenType type, std::string value, Meta mt, bool hasPrecedingSpace, bool hasFollowingSpace):
    type(type), value(value), mt(mt), hasPrecedingSpace(hasPrecedingSpace), hasFollowingSpace(hasFollowingSpace) {
    if (isReserved(value)) this->type = TokenType::RESERVED;
    if (value == "(") this->type = TokenType::L_PAREN;
    if (value == ")") this->type = TokenType::R_PAREN;
    if (value == "{") this->type = TokenType::L_BRACE;
    if (value == "}") this->type = TokenType::R_BRACE;
    if (value == "[") this->type = TokenType::L_BRACK;
    if (value == "]") this->type = TokenType::R_BRACK;
    if (value == "true" || value == "false") this->type = TokenType::BOOL;
}

bool Token::is(std::string str) {
    return value == str;
}

bool Token::is(char c) {
    return value.size() == 1 && value[0] == c;
}

bool Token::isEOF() {
    return type == TokenType::EOF_;
}

int Token::precedence() {
    if (hasFollowingSpace == false) {
        return -1;
    }
    if (value == "=")
        return 10;
    if (value == "&&" || value == "||")
        return 20;
    if (value == "==" || value == "!=")
        return 30;
    if (value == "<" || value == "<=" || value == ">" || value == ">=")
        return 40;
    if (value == "+" || value == "-")
        return 50;
    if (value == "*" || value == "/")
        return 60;
    return -1;
}

std::string Token::toString() {
    return fmt::format("`('{}' {} {}:{})`", value, mt.filename, mt.line, mt.offset);
}
