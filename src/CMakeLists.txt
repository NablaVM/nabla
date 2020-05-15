cmake_minimum_required(VERSION 3.8)

project(nablaLang)

set(CMAKE_CXX_STANDARD 17)

# Build options
option(BUILD_SOLACE         "Build solace parser"            ON)
option(BUILD_NABLA          "Build nabla application"        ON)
option(COMPILE_TESTS        "Execute compile-time tests"     ON)
option(SOLACE_DEBUG_OUTPUT  "Enable solace debug output"     OFF)
option(VM_DEBUG_OUTPUT      "Enable VM debug output"         OFF)

# Add a macro for the vm if we want debug output
if(VM_DEBUG_OUTPUT)
    add_definitions(-DNABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT)
endif()

# Add a macro for solace if we want debug output
if(SOLACE_DEBUG_OUTPUT)
    add_definitions(-DNABLA_SOLACE_DEBUG_OUTPUT)
endif()

# Get version info
include(${CMAKE_SOURCE_DIR}/cmake/NablaVersion.cmake)

# Discover the platform that we are building on
include(${CMAKE_SOURCE_DIR}/cmake/Platform.cmake)

# Set Cmake Variables for CPPUTest
include(${CMAKE_SOURCE_DIR}/cmake/FindCppuTest.cmake)

# Set CMake Variables for INCLUDES HEADERS and SOURCES for libc
include(${CMAKE_SOURCE_DIR}/libc/NablaLibc.cmake)

# Set CMake Variables for INCLUDES HEADERS and SOURCES for libcpp
include(${CMAKE_SOURCE_DIR}/libcpp/NablaLibcpp.cmake)

# -------------------------------------------------
#   All Nabla tests
# -------------------------------------------------
if(COMPILE_TESTS)
    add_subdirectory(tests)
endif()

# -------------------------------------------------
#   Solace Assembler
# -------------------------------------------------
if(BUILD_SOLACE)
    add_subdirectory(apps/solace)
endif()

# -------------------------------------------------
#   Nabla Application
# -------------------------------------------------
if(BUILD_NABLA)
    add_subdirectory(apps/nabla)
endif()