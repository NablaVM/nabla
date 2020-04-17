cmake_minimum_required(VERSION 3.8)

project(nablaVm C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/stack)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/vm)

set(NABLA_VM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/stack/stack.c
    ${CMAKE_CURRENT_SOURCE_DIR}/vm/vm.c
)

add_executable(nablaVm main.c ${NABLA_VM_SOURCES})