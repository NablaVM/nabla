cmake_minimum_required(VERSION 3.8)

project(nablaCompiler)

include(${CMAKE_SOURCE_DIR}/cmake/FindCppuTest.cmake)

include_directories(lex)

set(NABLA_COMPILER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/lex/Parser.cpp
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests/)

add_executable(nabla-compiler main.cpp ${NABLA_COMPILER_SOURCES})

