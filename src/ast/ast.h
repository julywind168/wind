#ifndef WIND_AST_H
#define WIND_AST_H


#include "expr/Expr.h"
#include "expr/Literal/StrLit.h"
#include "expr/Literal/BoolLit.h"
#include "expr/Literal/IntLit.h"
#include "expr/Literal/FloatLit.h"

#include "expr/Identifier.h"
#include "expr/Dot.h"
#include "expr/Equal.h"
#include "expr/Infix.h"
#include "expr/Unary.h"
#include "expr/Dot.h"
#include "expr/Index.h"
#include "expr/Block.h"
#include "expr/Call.h"
#include "expr/Chunk.h"

#include "expr/S/Callee.h"
#include "expr/S/Extern.h"
#include "expr/S/Func.h"
#include "expr/S/Var.h"
#include "expr/S/If.h"
#include "expr/S/Loop.h"
#include "expr/S/Cond.h"
#include "expr/S/Struct.h"
#include "expr/S/Impl.h"
#include "expr/S/Protocol.h"
#include "expr/S/Alloc.h"
#include "expr/S/Sizeof.h"
#include "expr/S/Cast.h"
#include "expr/S/TupleConstr.h"

#include "expr/Block/StructConstr.h"
#include "expr/Brack/ArrayConstr.h"

#include "Env.h"


#endif