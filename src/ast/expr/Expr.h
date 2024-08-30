#ifndef WIND_EXPR_H
#define WIND_EXPR_H

#include <string>
#include "core/Type.h"
#include "fmt/format.h"
#include "ast/Env.h"
#include "ast/expr/S/Callee.h"
#include "core/CompileCtx.h"
#include "lexer/Token.h"

std::unique_ptr<Expr> parseString(std::shared_ptr<Env> env, std::string filename, std::string str);


enum class NodeType {
    // Program
    CHUNK,
    // Block
    STRUCT_CONSTR,
    // Bracket
    ARRAY_CONSTR,
    // S Expression
    TUPLE_CONSTR,
    CALL,
    STRUCT,
    VAR,
    BLOCK,
    FUNC,
    EXTERN,
    IF,
    LOOP,
    COND,
    MATCH,
    IMPL,
    PROTOCOL,
    // S Macro Function
    ALLOC,
    SIZEOF,
    CAST,
    LOAD,
    STORE,
    // Atoms
    IDENTIFIER,
    DOT,        // extend identifier
    INDEX,      // extend identifier
    EQUAL,      // `=`
    INFIX,
    UNARY,
    // literal
    STR_LIT,
    INT_LIT,
    FLOAT_LIT,
    BOOL_LIT,
};

bool isEqual(std::shared_ptr<wind::Type> ty, std::shared_ptr<wind::Type> ty2);
bool isEqual(std::vector<std::shared_ptr<wind::Type>> tyList, std::vector<std::shared_ptr<wind::Type>> tyList2);

bool invalidTypeCast(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> from, std::shared_ptr<wind::Type> to);

class Expr {
public:
    virtual ~Expr() = default;
    virtual NodeType nodeTy() const = 0;
    virtual std::string toString() const = 0;
    virtual bool isLiteral() { return false; }
    virtual bool isIdentifier() { return false; }
    virtual llvm::Value* codegen(CompileCtx &ctx) { return nullptr; }
    virtual llvm::Value* getVariableAddress(CompileCtx &ctx) { return nullptr; }
    virtual void typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy = nullptr) {
        if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
            throw std::runtime_error(fmt::format("Expr::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
        }
    }
    virtual std::shared_ptr<Callee> getCallee(std::vector<std::shared_ptr<wind::Type>> types) { return nullptr; }
    virtual llvm::Value* createCall(CompileCtx &ctx, std::vector<std::shared_ptr<wind::Type>> types, std::vector<llvm::Value*> args) { throw std::runtime_error("Invalid callee"); }
    virtual void markTy(std::shared_ptr<wind::Type> ty) { this->ty = ty; }

public:
    Meta mt = Meta();
    std::shared_ptr<wind::Type> ty = wind::Type::VOID;

    std::string ty2Str() const {
        return ty ? "'" + ty->toString() : "";
    }
    std::string getSourceCode() {
        return mt.source;
    }

    void orphanCheck() {
        if (nodeTy() == NodeType::VAR) {
            panic("Orphan variable");
        } 
    }

    void panic(std::string msg) {
        throw std::runtime_error(fmt::format("{}:{}:{}: {} @source`{}`", mt.filename, mt.line, mt.offset, msg, mt.source));
    }
};


#endif
