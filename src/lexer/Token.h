#ifndef WIND_TOKEN_H
#define WIND_TOKEN_H

#include <cstddef>
#include <string>


enum class TokenType {
    INT, FLOAT, BOOL, STRING,  // literal
    RESERVED,                  // reserved
    IDENT,                     // identifier
    L_BRACE, R_BRACE,          // { }
    L_PAREN, R_PAREN,          // ( )
    L_BRACK, R_BRACK,          // [ ]
    OTHER,                     // + - * / < > <= == != >= . ; @ & ... ..<
    EOF_                       // eof
};

struct Meta {
    std::string filename = "";
    size_t line = 0;
    size_t offset = 0;
    std::string source = "";
};

class Token {
public:
    TokenType type;
    std::string value;
    Meta mt;
    bool hasPrecedingSpace;
    bool hasFollowingSpace;

    Token() {};
    Token(TokenType type, std::string value, Meta mt, bool hasPrecedingSpace, bool hasFollowingSpace);
    bool is(std::string str);
    bool is(char c);
    bool isEOF();
    int precedence();
    std::string toString();
};


#endif
