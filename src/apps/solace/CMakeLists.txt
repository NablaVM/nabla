cmake_minimum_required(VERSION 3.8)

project(solace)

# Needed for VM instructions
include_directories(${NABLA_LIBC_INCLUDES})

include_directories(${NABLA_LIBCPP_INCLUDES})

set(SOLACE_COMPILER_SOURCES
    ${NABLA_LIBCPP_SOURCES}
    ${CMAKE_CURRENT_SOURCE_DIR}/solace.cpp
)

if(COMPILE_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests/)
endif()

add_custom_target(copy-solace-asm ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/solace_code ${CMAKE_BINARY_DIR}/solace_code
)

add_executable(solace main.cpp ${SOLACE_COMPILER_SOURCES})

