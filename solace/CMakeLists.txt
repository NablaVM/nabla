cmake_minimum_required(VERSION 3.8)

project(solace)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

set(SOLACE_COMPILER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/solace.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bytegen.cpp
)

#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests/)

add_custom_target(copy-solace-asm ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/solace_code ${CMAKE_BINARY_DIR}/solace_code
)

add_executable(solace main.cpp ${SOLACE_COMPILER_SOURCES})

