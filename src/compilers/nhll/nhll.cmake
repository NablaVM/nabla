BISON_TARGET(nhll_parser
            ${NHLL_COMPILER_DIR}/nhll_parser.yy
            ${CMAKE_CURRENT_BINARY_DIR}/nhll_parser.tab.cc)

FLEX_TARGET(nhll_lexer
            ${NHLL_COMPILER_DIR}/nhll_lexer.l
            ${CMAKE_CURRENT_BINARY_DIR}/nhll_lexer.yy.cc)

ADD_FLEX_BISON_DEPENDENCY(nhll_lexer nhll_parser)

set(NHLL_INCLUDE_DIRS
    ${NHLL_COMPILER_DIR}/codegen
    ${NHLL_COMPILER_DIR}/preprocessor
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${FLEX_INCLUDE_DIRS}
)

set(NHLL_COMPILER_HEADERS
    ${NHLL_COMPILER_DIR}
    ${NHLL_COMPILER_DIR}/preprocessor/Preprocessor.hpp
    ${NHLL_COMPILER_DIR}/codegen/CodeGen.hpp
    ${NHLL_COMPILER_DIR}/nhll.hpp
    ${NHLL_COMPILER_DIR}/nhll_driver.hpp
    ${NHLL_COMPILER_DIR}/nhll_scanner.hpp
    ${NHLL_COMPILER_DIR}/nhll_postfix.hpp
)

set(NHLL_COMPILER_SOURCES
    ${NHLL_COMPILER_DIR}/preprocessor/Preprocessor.cpp
    ${NHLL_COMPILER_DIR}/codegen/CodeGen.cpp
    ${NHLL_COMPILER_DIR}/nhll.cpp
    ${NHLL_COMPILER_DIR}/nhll_driver.cpp
    ${NHLL_COMPILER_DIR}/nhll_postfix.cpp
    ${FLEX_nhll_lexer_OUTPUTS}
    ${BISON_nhll_parser_OUTPUTS}
)

