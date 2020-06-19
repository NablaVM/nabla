BISON_TARGET(del_parser
            ${DEL_COMPILER_DIR}/del_parser.yy
            ${CMAKE_CURRENT_BINARY_DIR}/del_parser.tab.cc)

FLEX_TARGET(del_lexer
            ${DEL_COMPILER_DIR}/del_lexer.l
            ${CMAKE_CURRENT_BINARY_DIR}/del_lexer.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(del_lexer del_parser)

set(DEL_INCLUDE_DIRS
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${DEL_COMPILER_DIR}/ast
    ${DEL_COMPILER_DIR}/codegen
    ${DEL_COMPILER_DIR}/codegen/asm
    ${DEL_COMPILER_DIR}/codegen/codeblocks
    ${DEL_COMPILER_DIR}/intermediate
    ${DEL_COMPILER_DIR}/managers
    ${DEL_COMPILER_DIR}/preprocessor
    ${DEL_COMPILER_DIR}/semantics
    ${FLEX_INCLUDE_DIRS}
)

set(DEL_COMPILER_HEADERS
    ${DEL_COMPILER_DIR}
    ${DEL_COMPILER_DIR}/ast/Ast.hpp
    ${DEL_COMPILER_DIR}/ast/Types.hpp

    ${DEL_COMPILER_DIR}/codegen/Codegen.hpp
    ${DEL_COMPILER_DIR}/codegen/Generator.hpp
    ${DEL_COMPILER_DIR}/codegen/asm/AsmMath.hpp
    ${DEL_COMPILER_DIR}/codegen/asm/AsmStoreLoad.hpp
    ${DEL_COMPILER_DIR}/codegen/asm/AsmSupport.hpp
    ${DEL_COMPILER_DIR}/codegen/codeblocks/Alloc.hpp
    ${DEL_COMPILER_DIR}/codegen/codeblocks/BlockAggregator.hpp
    ${DEL_COMPILER_DIR}/codegen/codeblocks/Codeblock.hpp
    ${DEL_COMPILER_DIR}/codegen/codeblocks/ConditionalContext.hpp
    ${DEL_COMPILER_DIR}/codegen/codeblocks/Function.hpp
    ${DEL_COMPILER_DIR}/codegen/codeblocks/LoadStore.hpp
    ${DEL_COMPILER_DIR}/codegen/codeblocks/Operations.hpp
    ${DEL_COMPILER_DIR}/codegen/codeblocks/Primitives.hpp

    ${DEL_COMPILER_DIR}/intermediate/Intermediate.hpp
    ${DEL_COMPILER_DIR}/intermediate/IntermediateTypes.hpp

    ${DEL_COMPILER_DIR}/managers/Errors.hpp
    ${DEL_COMPILER_DIR}/managers/Memory.hpp
    ${DEL_COMPILER_DIR}/managers/SymbolTable.hpp

    ${DEL_COMPILER_DIR}/preprocessor/Preprocessor.hpp

    ${DEL_COMPILER_DIR}/semantics/Analyzer.hpp
    ${DEL_COMPILER_DIR}/semantics/EnDecode.hpp

    ${DEL_COMPILER_DIR}/del_driver.hpp
    ${DEL_COMPILER_DIR}/del_scanner.hpp

    ${DEL_COMPILER_DIR}/SystemSettings.hpp
)

set(DEL_COMPILER_SOURCES
    ${DEL_COMPILER_DIR}/ast/Ast.cpp

    ${DEL_COMPILER_DIR}/codegen/Codegen.cpp
    ${DEL_COMPILER_DIR}/codegen/Generator.cpp
    ${DEL_COMPILER_DIR}/codegen/asm/AsmSupport.cpp

    ${DEL_COMPILER_DIR}/intermediate/Intermediate.cpp

    ${DEL_COMPILER_DIR}/managers/Errors.cpp
    ${DEL_COMPILER_DIR}/managers/SymbolTable.cpp
    ${DEL_COMPILER_DIR}/managers/Memory.cpp

    ${DEL_COMPILER_DIR}/preprocessor/Preprocessor.cpp

    ${DEL_COMPILER_DIR}/semantics/Analyzer.cpp
    ${DEL_COMPILER_DIR}/semantics/EnDecode.cpp

    ${DEL_COMPILER_DIR}/del_driver.cpp
    
    ${FLEX_del_lexer_OUTPUTS}
    ${BISON_del_parser_OUTPUTS}
)

