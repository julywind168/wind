#include "Parser.h"

std::unique_ptr<Expr> Parser::parseSExpr(std::shared_ptr<Env> env) {
    auto ident = peek()->value;
    if (ident == "func") {
        return parseFunc(env, true);
    } else if (ident == "extern") {
        return parseExtern(env);
    } else if (ident == "if") {
        return parseIf(env);
    } else if (ident == "var" || ident == "let") {
        return parseVar(env);
    } else if (ident == "loop") {
        return parseLoop(env);
    } else if (ident == "cond") {
        return parseCond(env);
    } else if (ident == "struct") {
        return parseStruct(env);
    } else if (ident == "impl") {
        return parseImpl(env);
    } else if (ident == "protocol") {
        return parseProtocol(env);
    } else if (ident == "sizeof") {
        return parseSizeof(env);
    } else if (ident == "alloc") {
        return parseAlloc(env);
    } else if (ident == "cast") {
        return parseCast(env);
    } else {
        if (peek(2)->value == "<" && peek(2)->hasPrecedingSpace == false) {
            return parseGenericsCall(env);
        }
        eat('(');
        auto first = parseExpr(env);
        if (tryEat(',')) {
            return parseTupleConstr(env, std::move(first));
        }
        auto exprs = parseExprList(env);
        exprs.insert(exprs.begin(), std::move(first));
        eat(')');
        // `(value)` -> `value`
        if (exprs.size() == 1 && dynamic_cast<Identifier*>(exprs[0].get()) == nullptr) {
            return std::move(exprs[0]);
        } else {
            auto callable = std::move(exprs[0]);
            exprs.erase(exprs.begin());
            return std::make_unique<Call>(std::move(callable), std::vector<std::shared_ptr<wind::Type>>(), std::move(exprs));
        }
    }
}

// (1, 2, 3)
std::unique_ptr<Expr> Parser::parseTupleConstr(std::shared_ptr<Env> env, std::unique_ptr<Expr> first) {
    std::vector<std::unique_ptr<Expr>> elements;
    elements.push_back(std::move(first));
    while (!curTk->is(')')) {
        auto expr = parseExpr(env);
        if (!expr) {
            throw std::runtime_error("tuple expected `(value, value, ...)`");
        }
        elements.push_back(std::move(expr));
        if (!tryEat(',')) {
            break;
        }
    }
    eat(')');
    return std::make_unique<TupleConstr>(std::move(elements));
}

// (add<i32> 1 2)
std::unique_ptr<Expr> Parser::parseGenericsCall(std::shared_ptr<Env> env) {
    eat('(');
    auto callable = parseIdentExpr(env);
    eat('<');
    auto generics = parseTyList(env, ',');
    eat('>');
    auto params = parseExprList(env);
    eat(')');
    return std::make_unique<Call>(std::move(callable), generics, std::move(params));
}

// (func (foo x'i32 y'i32)'i32 -> (x + y))
std::unique_ptr<Func> Parser::parseFunc(std::shared_ptr<Env> env, bool requiredBody, std::vector<std::shared_ptr<wind::BoxedType>> structGenerics) {
    eat('(');
    eat("func");
    auto myEnv = std::make_shared<Env>(env);
    auto callee = parseCallee(myEnv, structGenerics);
    std::unique_ptr<Expr> body = nullptr;
    if (requiredBody) {
        eat("->");
        body = parseExpr(myEnv);
    }
    eat(')');
    auto e = std::make_unique<Func>(myEnv, callee, std::move(body));
    env->define(callee->name, Symbol::function_(callee->name, e.get()));
    return e;
}

// (extern (foo x'i32 y'i32)'i32)
std::unique_ptr<Expr> Parser::parseExtern(std::shared_ptr<Env> env) {
    eat('(');
    eat("extern");
    auto callee = parseCallee(env);
    eat(')');
    auto e =  std::make_unique<Extern>(callee);
    env->define(callee->name, Symbol::function_(callee->name, e.get()));
    return e;
}

// (if test then else)
std::unique_ptr<Expr> Parser::parseIf(std::shared_ptr<Env> env) {
    eat('(');
    eat("if");
    auto cond = parseExpr(env);
    auto then = parseExpr(env);
    auto else_ = parseExpr(env);
    if (!then) {
        throw std::runtime_error("If expected `(if test then [else])`");
    }
    eat(')');
    return std::make_unique<If>(std::move(cond), std::move(then), std::move(else_));
}

// (var/let name't init)
std::unique_ptr<Expr> Parser::parseVar(std::shared_ptr<Env> env) {
    eat('(');
    auto const_ = advance() == "let";
    auto name = parseIdent();
    auto hintTy = tryParseMarkedTy(env);
    eat('=');
    auto init = parseExpr(env);
    if (!init) {
        throw std::runtime_error("Var expected `(var/let name init)`");
    }
    eat(')');
    return std::make_unique<Var>(const_, name, hintTy, std::move(init));
}

// (loop [init] test body)
std::unique_ptr<Expr> Parser::parseLoop(std::shared_ptr<Env> env) {
    eat('(');
    eat("loop");
    auto init = parseExpr(env);
    auto test = parseExpr(env);
    if (!test) {
        throw std::runtime_error("loop expected `(loop [init] test body)`");
    }
    auto body = parseExpr(env);
    eat(')');
    if (!body) {
        return std::make_unique<Loop>(nullptr, std::move(init), std::move(test));
    } else {
        return std::make_unique<Loop>(std::move(init), std::move(test), std::move(body));
    }
}

// (cond test => then test2 => then2 ...)
std::unique_ptr<Expr> Parser::parseCond(std::shared_ptr<Env> env) {
    eat('(');
    eat("cond");
    std::vector<std::shared_ptr<Env>> caseEnvs;
    std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> cases;
    while (!curTk->is(')')) {
        auto caseEnv = std::make_shared<Env>(env);
        caseEnvs.push_back(caseEnv);
        std::unique_ptr<Expr> test = parseExpr(caseEnv);
        eat("=>");
        std::unique_ptr<Expr> then = parseExpr(caseEnv);
        if (!test || !then) {
            throw std::runtime_error("invalid cond");
        }
        cases.push_back(std::make_pair(std::move(test), std::move(then)));
    }
    if (cases.empty()) {
        throw std::runtime_error("cond expected `(cond test => then test2 => then2 ...)`");
    }
    eat(')');
    return std::make_unique<Cond>(caseEnvs, std::move(cases));
}


// <T, T2, ...>
void Parser::parseGenericDecl(std::string structName, std::shared_ptr<Env> env, std::vector<std::pair<std::string, std::shared_ptr<wind::BoxedType>>>& generics) {
    eat('<');
    auto idents = parseIdentList(',');
    eat(">");
    auto s = env->lookup(structName);
    if (s) {
        if (idents.size() != s->t.generics.size()) {
            throw std::runtime_error(fmt::format("struct `{}` generics size mismatch", structName));
        }
        for (size_t i = 0; i < idents.size(); i++) {
            auto ident = idents[i];
            if (ident != s->t.generics[i].first) {
                throw std::runtime_error(fmt::format("struct `{}` generic name mismatch", structName));
            }
            auto ty = s->t.generics[i].second;
            env->define(ident, Symbol::boxtype_(ident, ty));
            generics.push_back(std::make_pair(ident, ty));
        }
    } else {
        for (auto ident : idents) {
            auto ty = std::make_shared<wind::BoxedType>();
            env->define(ident, Symbol::boxtype_(ident, ty));
            generics.push_back(std::make_pair(ident, ty));
        }
    }
}

// (struct Point<T, T2, ...> (x'i32, y'i32))
std::unique_ptr<Expr> Parser::parseStruct(std::shared_ptr<Env> env) {
    eat('(');
    eat("struct");
    auto name = parseIdent();
    auto myEnv = std::make_shared<Env>(env);
    auto s = env->lookup(name);
    if (s && s->ty != SymbolType::TYPE) {
        throw std::runtime_error(fmt::format("struct `{}` already defined", name));
    }
    std::vector<std::pair<std::string, std::shared_ptr<wind::BoxedType>>> generics;
    std::vector<std::shared_ptr<wind::Type>> filedTys;
    std::vector<Field> fields;
    if (curTk->is('<')) {
        parseGenericDecl(name, myEnv, generics);
    }
    eat('(');
    while (!curTk->is(')')) {
        auto fieldName = parseIdent();
        auto fieldTy = tryParseMarkedTy(myEnv);
        if (!fieldTy) {
            throw std::runtime_error("struct field expected a type");
        }
        filedTys.push_back(fieldTy);
        fields.push_back({fields.size(), fieldName, fieldTy});
        if (!tryEat(',')) {
            break;
        }
    }
    eat(')');
    eat(')');
    wind::NamedType::defineStruct(name, filedTys);

    if (s) {
        if (s->t.fields.size() != 0) {
            throw std::runtime_error("struct " + name + " redefinition");
        }
        s->t.fields = fields;
    } else {
        env->define(name, Symbol::type_(name, generics, fields, {}, {}));
    }
    return std::make_unique<Struct>(myEnv, name, generics, fields);
}


// (impl `type` : proto, proto2 func func2 ...)
std::unique_ptr<Expr> Parser::parseImpl(std::shared_ptr<Env> env) {
    eat('(');
    eat("impl");
    auto myEnv = std::make_shared<Env>(env);
    std::string tyName = parseIdent();
    std::vector<std::pair<std::string, std::shared_ptr<wind::BoxedType>>> generics;
    if (curTk->is('<')) {
        parseGenericDecl(tyName, myEnv, generics);
    }
    std::vector<std::string> protocols;
    std::vector<std::unique_ptr<Func>> funcs;
    if (tryEat(':')) {
        while (true) {
            protocols.push_back(parseIdent());
            if (!tryEat(',')) {
                break;
            }
        }
    }
    std::vector<std::shared_ptr<wind::BoxedType>> structGenerics;
    for (auto g : generics) {
        structGenerics.push_back(g.second);
    }

    while (curTk->is('(')) {
        auto f = parseFunc(myEnv, true, structGenerics);
        if (f) {
            funcs.push_back(std::move(f));
        } else {
            break;
        }
    }
    eat(')');

    // mark self type, don't include generics info
    auto selfTy = wind::getType(tyName, {});
    if (!selfTy->isPrimitive()) {
        selfTy = selfTy->ref();
    }
    for (auto& f : funcs) {
        f->callee->markSelfTy(selfTy);
    }

    // merge impl
    if (auto s = env->lookup(tyName)) {
        if (s->ty != SymbolType::TYPE) {
            throw std::runtime_error("type " + tyName + " not found");
        }
        for (auto &f : funcs) {
            s->t.methods.push_back(f.get());
        }
        for (auto protocol : protocols) {
            s->t.protocols.push_back(protocol);
        }
    } else {
        std::vector<IFunc*> ifuncs;
        for (auto &f : funcs) {
            ifuncs.push_back(f.get());
        }
        env->define(tyName, Symbol::type_(tyName, generics, {}, ifuncs, protocols));
    }
    return std::make_unique<Impl>(tyName, generics, protocols, std::move(funcs));
}

// (protocol `name` func func2 ...)
std::unique_ptr<Expr> Parser::parseProtocol(std::shared_ptr<Env> env) {
    eat('(');
    eat("protocol");
    auto name = parseIdent();
    std::vector<std::shared_ptr<Callee>> callees;
    while (curTk->is('(')) {
        auto f = parseFunc(env, false);
        if (f) {
            f->callee->markSelfTy(wind::Type::PTR);
            callees.push_back(f->callee);
        } else {
            break;
        }
    }
    eat(')');
    wind::NamedType::defineProtocol(name);
    auto p = std::make_unique<Protocol_>(env, name, callees);
    env->define(name, Symbol::protocol_(name, p.get()));
    return std::move(p);
}

// (sizeof type)
std::unique_ptr<Expr> Parser::parseSizeof(std::shared_ptr<Env> env) {
    eat('(');
    eat("sizeof");
    auto ty = parseTy(env);
    eat(')');
    return std::make_unique<Sizeof>(std::move(ty));
}

// (alloc type size)
std::unique_ptr<Expr> Parser::parseAlloc(std::shared_ptr<Env> env) {
    eat('(');
    eat("alloc");
    auto ty = parseTy(env);
    auto size = parseExpr(env);
    eat(')');
    return std::make_unique<Alloc>(ty, std::move(size));
}

// (cast dstTy source)
std::unique_ptr<Expr> Parser::parseCast(std::shared_ptr<Env> env) {
    eat('(');
    eat("cast");
    auto dstTy = parseTy(env);
    auto source = parseExpr(env);
    eat(')');
    return std::make_unique<Cast>(dstTy, std::move(source));
}

// (foo<T, T2, ...> x'i32 y'i32)'i32
std::shared_ptr<Callee> Parser::parseCallee(std::shared_ptr<Env> env, std::vector<std::shared_ptr<wind::BoxedType>> structGenerics) {
    eat('(');
    auto name = parseIdent();
    std::vector<std::shared_ptr<wind::BoxedType>> generics;
    if (curTk->is('<')) {
        if (!structGenerics.empty()) {
            throw std::runtime_error("Cannot have generics function in impl");
        }
        eat("<");
        auto idents = parseIdentList(',');
        for (auto ident : idents) {
            auto ty = std::make_shared<wind::BoxedType>();
            generics.push_back(ty);
            env->define(ident, Symbol::boxtype_(ident, ty));
        }
        eat(">");
    }
    std::vector<Param> params;
    bool isVarArg = false;
    while (!curTk->is(')')) {
        if (curTk->type == TokenType::IDENT) {
            auto paramName = curTk->value;
            advance();
            auto paramTy = tryParseMarkedTy(env);
            if (paramName != "self" && !paramTy) {
                throw std::runtime_error("expected type after parameter name");
            }
            params.push_back(Param{paramName, paramTy});
        } else if (curTk->value == "...") {
            isVarArg = true;
            advance();
            break;
        } else {
            throw std::runtime_error("func param error");
        }
    }
    eat(')');
    auto retTy = tryParseMarkedTy(env, wind::Type::VOID);
    return std::make_shared<Callee>(name, generics.empty() ? structGenerics : generics, params, retTy, isVarArg);
}
