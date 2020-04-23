cmake_minimum_required(VERSION 3.8)

project(harp)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libc/vm)
include_directories(${CMAKE_SOURCE_DIR}/libc/stack)
include_directories(${CMAKE_SOURCE_DIR}/libc/binloader)

# Needed to work with the vm
include_directories(${CMAKE_SOURCE_DIR}/apps/vm)

set(HARP_DECOMPILER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/Analyzer.cpp
    ${CMAKE_SOURCE_DIR}/libc/binloader/binloader.c
    ${CMAKE_SOURCE_DIR}/libc/stack/stack.c
    ${CMAKE_SOURCE_DIR}/apps/vm/vm.c
)


add_executable(harp main.cpp ${HARP_DECOMPILER_SOURCES})

