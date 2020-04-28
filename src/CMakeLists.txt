cmake_minimum_required(VERSION 3.8)

project(nablaLang)

set(CMAKE_CXX_STANDARD 11)

option(BUILD_SOLACE         "Build solace parser"        ON)
option(BUILD_HARP           "Build harp application"     ON)
option(BUILD_NABLA          "Build nabla application"    ON)
option(COMPILE_TESTS        "Execute compile-time tests" ON)
option(VM_DEBUG_OUTPUT      "Enable VM debug output"     ON)

if(VM_DEBUG_OUTPUT)
    add_definitions(-DNABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/FindCppuTest.cmake)

# Set CMake Variables for INCLUDES HEADERS and SOURCES for libc
include(${CMAKE_SOURCE_DIR}/libc/NablaLibc.cmake)

# Set CMake Variables for INCLUDES HEADERS and SOURCES for libcpp
include(${CMAKE_SOURCE_DIR}/libcpp/NablaLibcpp.cmake)

# -------------------------------------------------
#   Solace Assembler
# -------------------------------------------------
if(BUILD_SOLACE)
    add_subdirectory(apps/solace)
endif()

# -------------------------------------------------
#   Harp Analyzer
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
#   All Nabla tests
# -------------------------------------------------
if(COMPILE_TESTS)
    add_subdirectory(tests)
endif()