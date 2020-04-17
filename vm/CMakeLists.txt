cmake_minimum_required(VERSION 3.8)

project(nablaVm C)

set(NABLA_VM_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/vm.c
)

add_executable(nablaVm main.c ${NABLA_VM_SOURCES})