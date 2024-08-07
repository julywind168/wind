#ifndef WIND_LEXER_H
#define WIND_LEXER_H

#include "Token.h"
#include <cstddef>
#include <string>
#include <vector>


std::vector<Token> tokenize(std::string filename, std::string text);



#endif
