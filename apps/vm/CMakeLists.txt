cmake_minimum_required(VERSION 3.8)

project(nablaVm C)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libc/stack)
include_directories(${CMAKE_SOURCE_DIR}/libc/binloader)
include_directories(${CMAKE_SOURCE_DIR}/libc/vm)

set(NABLA_VM_SOURCES
    ${CMAKE_SOURCE_DIR}/libc/binloader/binloader.c
    ${CMAKE_SOURCE_DIR}/libc/stack/stack.c
    ${CMAKE_SOURCE_DIR}/apps/vm/vm.c
)

add_executable(nablaVm main.c ${NABLA_VM_SOURCES})