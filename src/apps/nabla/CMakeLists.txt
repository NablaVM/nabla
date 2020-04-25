cmake_minimum_required(VERSION 3.8)

project(nabla)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libc/binloader)
include_directories(${CMAKE_SOURCE_DIR}/libc/stack)
include_directories(${CMAKE_SOURCE_DIR}/libc/vm)

set(NABLA_APP_SOURCES
    ${CMAKE_SOURCE_DIR}/libc/stack/stack.c
    ${CMAKE_SOURCE_DIR}/libc/binloader/binloader.c
    ${CMAKE_SOURCE_DIR}/libc/vm/vm.c

)

if(COMPILE_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/tests/)
endif()

add_executable(nabla main.cpp
                     BinExec.cpp ${NABLA_APP_SOURCES})