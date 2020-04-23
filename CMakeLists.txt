cmake_minimum_required(VERSION 3.8)

project(nablaLang)

option(BUILD_SOLACE         "Build solace parser"        ON)
option(BUILD_HARP           "Build harp application"     ON)
option(BUILD_VM             "Build nabla VM"             ON)
option(COMPILE_TESTS        "Execute compile-time tests" ON)

include(${CMAKE_SOURCE_DIR}/cmake/FindCppuTest.cmake)

# -------------------------------------------------
#   Solace Parser
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
#   Virtual Machine
# -------------------------------------------------
if(BUILD_VM)
    add_subdirectory(apps/vm)
endif()

# -------------------------------------------------
#   libc tests (Non-CPPU Tests)
# -------------------------------------------------
if(COMPILE_TESTS)
    add_subdirectory(libc/stack/test)
endif()