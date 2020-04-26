cmake_minimum_required(VERSION 3.8)

project(nablaLang)

set(CMAKE_CXX_STANDARD 11)

option(BUILD_SOLACE         "Build solace parser"        ON)
option(BUILD_HARP           "Build harp application"     ON)
option(BUILD_NABLA          "Build nabla application"    ON)
option(COMPILE_TESTS        "Execute compile-time tests" ON)

include(${CMAKE_SOURCE_DIR}/cmake/FindCppuTest.cmake)

# -------------------------------------------------
#   Solace Assembler
# -------------------------------------------------
if(BUILD_SOLACE)
    add_subdirectory(apps/solace)
endif()

# -------------------------------------------------
#   Harp decompiler
# -------------------------------------------------
if(BUILD_HARP)
    add_subdirectory(apps/harp)
endif()

# -------------------------------------------------
#   Nabla Application
# -------------------------------------------------
if(BUILD_NABLA)
    add_subdirectory(apps/nabla)
endif()

# -------------------------------------------------
#   libc tests (Non-CPPU Tests)
# -------------------------------------------------
if(COMPILE_TESTS)
    add_subdirectory(libc/stack/test)
endif()