cmake_minimum_required(VERSION 3.8)

project(harp)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${NABLA_LIBC_INCLUDES})

# Needed to work with the vm
include_directories(${CMAKE_SOURCE_DIR}/apps/vm)

set(HARP_DECOMPILER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Analyzer.cpp
    ${NABLA_LIBC_SOURCES}
)

add_executable(harp main.cpp ${HARP_DECOMPILER_SOURCES})

