cmake_minimum_required(VERSION 3.12)
project(wind)

set(CMAKE_CXX_COMPILER clang++)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# cxxflags
set(LLVM_CXXFLAGS "-D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -stdlib=libc++")
set(WIND_CXXFLAGS "-g -Wall -DFMT_HEADER_ONLY")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WIND_CXXFLAGS} ${LLVM_CXXFLAGS}")

set(SOURCE_FILES
    # util
    src/util/Util.cpp

    # lexer
    src/lexer/Token.cpp
    src/lexer/Lexer.cpp

    # ast
    src/ast/Env.cpp
    src/ast/expr/Expr.cpp
    src/ast/expr/literal/StrLit.cpp
    src/ast/expr/literal/BoolLit.cpp
    src/ast/expr/literal/IntLit.cpp
    src/ast/expr/literal/FloatLit.cpp
    src/ast/expr/Equal.cpp
    src/ast/expr/Infix.cpp
    src/ast/expr/Block.cpp
    src/ast/expr/Identifier.cpp
    src/ast/expr/Dot.cpp
    src/ast/expr/Index.cpp
    src/ast/expr/Call.cpp
    src/ast/expr/Unary.cpp

    src/ast/expr/S/Struct.cpp
    src/ast/expr/S/Callee.cpp
    src/ast/expr/S/Func.cpp
    src/ast/expr/S/Var.cpp
    src/ast/expr/S/If.cpp
    src/ast/expr/S/Loop.cpp
    src/ast/expr/S/Cond.cpp
    src/ast/expr/S/Impl.cpp
    src/ast/expr/S/Protocol.cpp
    src/ast/expr/S/Alloc.cpp
    src/ast/expr/S/Sizeof.cpp
    src/ast/expr/S/Cast.cpp
    src/ast/expr/S/TupleConstr.cpp

    src/ast/expr/Block/StructConstr.cpp
    src/ast/expr/Brack/ArrayConstr.cpp

    # parser
    src/parser/Parser.cpp
    src/parser/Parser+Util.cpp
    src/parser/Parser+Atom.cpp
    src/parser/Parser+S.cpp
    src/parser/Parser+Block.cpp
    src/parser/Parser+Brack.cpp

    # core
    src/core/types/itype.cpp
    src/core/types/named.cpp
    src/core/Type.cpp
    src/core/CompileCtx.cpp
    src/core/Core.cpp


    # compiler
    src/compiler/Compiler.cpp

    # main
    src/main/CmdArgs.cpp
    src/main/main.cpp

    # runtime
    src/runtime/runtime.cpp

    # std
    src/std/vm.cpp
    src/std/socket.cpp
)

# wind
add_executable(wind ${SOURCE_FILES})

target_include_directories(wind PRIVATE
    "${PROJECT_SOURCE_DIR}/src"
    "${PROJECT_SOURCE_DIR}/3rd"
    /opt/homebrew/opt/libgc/include
    /opt/homebrew/opt/llvm/include
)

target_link_libraries(wind PRIVATE
    /opt/homebrew/opt/libgc/lib/libgc.dylib
    /opt/homebrew/opt/llvm/lib/libLLVM.dylib
)
