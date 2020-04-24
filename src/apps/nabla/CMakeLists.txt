cmake_minimum_required(VERSION 3.8)

project(nabla)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libc/stack)
include_directories(${CMAKE_SOURCE_DIR}/libc/binloader)
include_directories(${CMAKE_SOURCE_DIR}/libc/vm)

set(NABLA_APP_SOURCES
    ${CMAKE_SOURCE_DIR}/libc/binloader/binloader.c
    ${CMAKE_SOURCE_DIR}/libc/stack/stack.c
    ${CMAKE_SOURCE_DIR}/libc/vm/vm.c

    BinExec.cpp
)

add_executable(nabla main.cpp ${NABLA_APP_SOURCES})