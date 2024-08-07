#include "Infix.h"

void Infix::markTy(std::shared_ptr<wind::Type> ty) {
    panic("Infix type depend on `left` and `right` expr, can't been mark");
}

bool isArithOp(const std::string& op) {
    return op == "+" || op == "-" || op == "*" || op == "/" || op == "%";
}

bool isLogicalOp(const std::string& op) {
    return op == "&&" || op == "||";
}

bool isCmpOp(const std::string& op) {
    return op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=";
}

void Infix::typecheck(std::shared_ptr<Env> env, std::shared_ptr<wind::Type> expectedTy) {
    if (isArithOp(op)) {
        auto who = left->isLiteral() ? right : left;
        auto other = who == left ? right : left;
        if (!who->isLiteral() && other->isLiteral()) {
            who->typecheck(env);
            other->typecheck(env, who->ty); // literal can conditional cast to another, if types dismatch
        } else {
            left->typecheck(env);
            right->typecheck(env);
        }
        ty = who->ty;
        // todo check types
    } else if (isLogicalOp(op)) {
        ty = wind::Type::BOOL;
        left->typecheck(env, ty);
        right->typecheck(env, ty);
    } else if (isCmpOp(op)) {
        ty = wind::Type::BOOL;
        left->typecheck(env);
        right->typecheck(env);
        // todo check compatibility
    } else {
        panic("Invalid infix op:" + op);
    }
    if (!isEqual(left->ty, right->ty)) {
        panic(fmt::format("Infix::typecheck: type mismatch left:{}, right:{}", left->ty->toString(), right->ty->toString()));
    }
    if (expectedTy && invalidTypeCast(env, ty, expectedTy)) {
        panic(fmt::format("Infix::typecheck failed, expected {}, but {}", expectedTy->toString(), ty ? ty->toString() : "void"));
    }
}

llvm::CmpInst::Predicate Infix::getCmpPred() {
    auto _ty = left->ty->get();
    if (op == "<" || op == ">") {
        if (_ty->isSigned()) return llvm::CmpInst::ICMP_SLT;
        if (_ty->isUnsigned()) return llvm::CmpInst::ICMP_ULT;
        assert(_ty->isFloat()); return llvm::CmpInst::FCMP_ULT;
    } else if (op == "<=" || op == ">=") {
        if (_ty->isSigned()) return llvm::CmpInst::ICMP_SLE;
        if (_ty->isUnsigned()) return llvm::CmpInst::ICMP_ULE;
        assert(_ty->isFloat()); return llvm::CmpInst::FCMP_ULE;
    } else if (op == "==") {
        return _ty->isFloat()? llvm::CmpInst::FCMP_UEQ : llvm::CmpInst::ICMP_EQ;
    } else {
        assert(op == "!=");
        return _ty->isFloat()? llvm::CmpInst::FCMP_UNE : llvm::CmpInst::ICMP_NE;
    }
}

llvm::Instruction::BinaryOps Infix::getOpc() {
    if (op == "+") { return ty->isInteger()? llvm::Instruction::BinaryOps::Add: llvm::Instruction::BinaryOps::FAdd; }
    if (op == "-") { return ty->isInteger()? llvm::Instruction::BinaryOps::Sub: llvm::Instruction::BinaryOps::FSub; }
    if (op == "*") { return ty->isInteger()? llvm::Instruction::BinaryOps::Mul: llvm::Instruction::BinaryOps::FMul; }
    if (op == "/") {
        if (ty->isSigned()) return llvm::Instruction::BinaryOps::SDiv;
        return ty->isUnsigned()? llvm::Instruction::BinaryOps::UDiv: llvm::Instruction::BinaryOps::FDiv;
    }
    if (op == "&&") { return llvm::Instruction::BinaryOps::And; }
    assert(op == "||");
    return llvm::Instruction::BinaryOps::Or;
}

llvm::Value* Infix::codegen(CompileCtx &ctx) {
    llvm::Value *l = left->codegen(ctx);
    llvm::Value *r = right->codegen(ctx);
    if (l && r) {
        if (isArithOp(op) || isLogicalOp(op)) {
            auto opc = getOpc();
            return ctx.builder->CreateBinOp(opc, l, r);
        } else if (isCmpOp(op)) {
            auto pred = getCmpPred();
            if (op == ">" || op == ">=")
                std::swap(l, r);
            return ctx.builder->CreateCmp(pred, l, r);
        }
        panic("Invalid infix op: " + op);
        return nullptr;
    }
    panic("infix left or right is null");
    return nullptr;
}
