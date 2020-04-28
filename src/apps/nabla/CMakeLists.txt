cmake_minimum_required(VERSION 3.8)

project(nabla)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${NABLA_LIBC_INCLUDES})

set(NABLA_APP_SOURCES
    ${NABLA_LIBC_SOURCES}
    BinExec.cpp
)

add_executable(nabla main.cpp ${NABLA_APP_SOURCES})