#include <cctype>
#include <cstdio>
#include <stdexcept>
#include <string>
#include <regex>
#include "fmt/core.h"
#include "Lexer.h"
#include "Token.h"

void LexPanic (std::string errmsg) {
    throw std::runtime_error("Lexer error: " + errmsg);
}

bool isValidIdentStartChar(char c) {
    return std::isalpha(c) or c == '_';
}

bool isValidIdentChar(char c) {
    return std::isalnum(c) or c == '_';
}

std::vector<Token> tokenize(std::string filename, std::string text) {
    std::vector<Token> tokens;
    size_t line = 1;
    size_t offset = 1;

    size_t index = 0;
    size_t count = text.size();
    char curChar = text[index];

    auto newline = [&line, &offset]() {
        line++;
        offset = 1;
    };

    auto advance = [&text, &curChar, &index, &offset](size_t step = 1) {
        index += step;
        offset += step;
        curChar = text[index];
    };

    auto nextChar = [&text, &index, count](size_t offset = 1) {
        if (index + offset < count)
            return text[index + offset];
        else
            return '\0';
    };

    auto arrived = [&index, count]() { return index == count; };

    bool space = false;

    auto takeSpace = [&space]() {
        auto tmp = space;
        space = false;
        return tmp;
    };

    auto fllowCharIsSpace = [&curChar]() { return std::isspace(curChar); };

    auto pushToken = [&tokens, &filename, &line, &offset, &takeSpace, &fllowCharIsSpace] (TokenType type, std::string value) {
        tokens.push_back(
            Token(type, value, Meta{filename, line, offset - value.size(), ""}, takeSpace(), fllowCharIsSpace())
        );
    };

    while (index < count) {
        std::string value;
        if (std::isdigit(curChar)) {
            while (std::isdigit(curChar) || curChar == '.') {
                value += curChar;
                advance();
            }
            auto ndot = std::count(value.begin(), value.end(), '.');
            if (ndot > 1)
                LexPanic(fmt::format("Invalid Number {}", value));
            else if (ndot == 1)
                pushToken(TokenType::FLOAT, value);
            else
                pushToken(TokenType::INT, value);
        } else if (isValidIdentStartChar(curChar)) {
            while (isValidIdentChar(curChar)) {
                value += curChar;
                advance();
            }
            pushToken(TokenType::IDENT, value);
        } else if (curChar == '"') {
            value = "\"";
            advance();
            while (curChar != '"') {
                value += curChar;
                advance();
                if (arrived())
                    LexPanic("Invalid string\n");
            }
            value += curChar; // add `"`
            advance();
            pushToken(TokenType::STRING, value);
        } else {
            bool discard = false;
            // comment
            if (curChar == '/' && nextChar() == '/') {
                while (curChar != '\n') {
                    advance();
                    if (arrived()) break;
                }
                discard = true;
            /* comment */
            } else if (curChar == '/' && nextChar() == '*') {
                advance(2);
                while (curChar != '*' || nextChar() != '/') {
                    advance();
                    if (arrived()) break;
                }
                advance(2);
                discard = true;
            } else if(std::isspace(curChar)) {
                if (curChar == '\n') {
                    advance();
                    newline();
                } else
                    advance();
                discard = true;
                space = true;
            } else if(curChar == '=' && nextChar() == '=') {
                advance(2);
                value = "==";
            } else if(curChar == '!' && nextChar() == '=') {
                advance(2);
                value = "!=";
            } else if(curChar == '<' && nextChar() == '=') {
                advance(2);
                value = "<=";
            } else if(curChar == '>' && nextChar() == '=') {
                advance(2);
                value = ">=";
            } else if(curChar == '&' && nextChar() == '&') {
                advance(2);
                value = "&&";
            } else if(curChar == '|' && nextChar() == '|') {
                advance(2);
                value = "||";
            } else if(curChar == '-' && nextChar() == '>') {
                advance(2);
                value = "->";
            } else if(curChar == '=' && nextChar() == '>') {
                advance(2);
                value = "=>";
            } else if(curChar == '.' && nextChar() == '.' && nextChar(2) == '.') {
                advance(3);
                value = "...";
            } else if(curChar == '.' && nextChar() == '.' && nextChar(2) == '<') {
                advance(3);
                value = "..<";
            } else {
                value = std::string(1, curChar);
                advance();
            }
            if (!discard) {
                pushToken(TokenType::OTHER, value);
            }
        }
    }
    if (text[count - 1] != '\n')
        line += 1;
    tokens.push_back(Token(TokenType::EOF_, "EOF", Meta{filename, line, 1, ""}, false, false));
    return tokens;
}
