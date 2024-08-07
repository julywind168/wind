#include "Parser.h"

std::shared_ptr<wind::Type> Parser::tryParseMarkedTy(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> defaultTy) {
    if (curTk->is('\'') && curTk->hasPrecedingSpace == false) {
        advance();
        return parseTy(env);
    } else {
        return defaultTy;
    }
}

std::shared_ptr<wind::Type> Parser::parseTy(std::shared_ptr<Env> env) {
    if (tryEat("'")) {
        return std::make_shared<wind::QuoteType>(parseTy(env));
    } else {
        std::string name = parseIdent();
        std::vector<std::shared_ptr<wind::Type>> parameters;
        if (curTk->is('<')) {
            eat("<");
            parameters = parseTyList(env, ',');
            eat(">");
        }
        if (parameters.empty()) {
            if (name == "void") {
                return wind::Type::VOID;
            } else if (name == "ptr") {
                return wind::Type::PTR;
            } else if (name == "bool") {
                return wind::Type::BOOL;
            } else if (name == "i8") {
                return wind::Type::I8;
            } else if (name == "i16") {
                return wind::Type::I16;
            } else if (name == "i32") {
                return wind::Type::I32;
            } else if (name == "i64") {
                return wind::Type::I64;
            } else if (name == "u8") {
                return wind::Type::U8;
            } else if (name == "u16") {
                return wind::Type::U16;
            } else if (name == "u32") {
                return wind::Type::U32;
            } else if (name == "u64") {
                return wind::Type::U64;
            } else if (name == "f32") {
                return wind::Type::F32;
            } else if (name == "f64") {
                return wind::Type::F64;
            } else {
                if (auto s = env->lookup(name)) {
                    if (s->ty == SymbolType::BOX_TYPE) { // generic type
                        return s->boxtype;
                    }
                }
                return std::make_shared<wind::NamedType>(name);
            }
        } else {
            return std::make_shared<wind::NamedType>(name, parameters);
        }
    }
}

std::vector<std::shared_ptr<wind::Type>> Parser::parseTyList(std::shared_ptr<Env> env, char separator) {
    std::vector<std::shared_ptr<wind::Type>> list;
    while (auto ty = parseTy(env)) {
        list.push_back(ty);
        if (curTk->is(separator)) {
            advance();
        } else {
            break;
        }
    }
    return list;
}

std::vector<std::string> Parser::parseIdentList(char separator) {
    std::vector<std::string> list;
    while(curTk->type == TokenType::IDENT) {
        list.push_back(curTk->value);
        advance();
        if (curTk->is(separator)) {
            advance();
        } else {
            break;
        }
    }
    return list;
}

std::string Parser::parseIdent() {
    if (curTk && curTk->type == TokenType::IDENT) {
        return advance();
    } else {
        throw std::runtime_error("ParseError: Expected identifier, got " + curTk->toString());
    }
}

bool Parser::tryEat(std::string str) {
    if (curTk->is(str)) {
        advance();
        return true;
    } else {
        return false;
    }
}

bool Parser::tryEat(char c) {
    if (curTk->is(c)) {
        advance();
        return true;
    } else {
        return false;
    }
}

void Parser::eat(std::string str) {
    if (curTk->value == str) {
        advance();
    } else {
        throw std::runtime_error("Expected " + str + ", got " + curTk->toString());
    }
}

void Parser::eat(char c) {
    if (curTk->value.size() == 1 && curTk->value[0] == c) {
        advance();
    } else {
        throw std::runtime_error("Expected " + std::string(1, c) + ", got " + curTk->toString());
    }
}

bool Parser::done() {
    return curTk == nullptr || curTk->type == TokenType::EOF_;
}