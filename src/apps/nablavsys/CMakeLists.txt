cmake_minimum_required(VERSION 3.8)

project(nablavsys)

include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${NABLA_LIBC_INCLUDES})
include_directories(${NABLA_LIBCPP_INCLUDES})

set(NABLA_VSYS_APP_SOURCES
    ${NABLA_LIBC_SOURCES}
    ${NABLA_LIBCPP_SOURCES}
)

add_executable(nablavsys main.cpp ${NABLA_VSYS_APP_SOURCES})