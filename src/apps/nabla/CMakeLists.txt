cmake_minimum_required(VERSION 3.8)

project(nabla)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${NABLA_LIBC_INCLUDES})
include_directories(${NABLA_LIBCPP_INCLUDES})

include_directories(BinExec)
include_directories(CliInterpreter)

set(NABLA_APP_SOURCES
    ${NABLA_LIBC_SOURCES}
    ${NABLA_LIBCPP_SOURCES}
    BinExec/BinExec.cpp
    CliInterpreter/CliInterpreter.cpp
)

add_executable(nabla main.cpp ${NABLA_APP_SOURCES})