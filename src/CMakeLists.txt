cmake_minimum_required(VERSION 3.8)

project(NablaApp)

#
#   Nabla Version Information
#
include(${CMAKE_SOURCE_DIR}/cmake/NablaVersion.cmake)

#
#   Platform Information
#
include(${CMAKE_SOURCE_DIR}/cmake/Platform.cmake)

set(CMAKE_CXX_STANDARD 17)

find_package(libnabla REQUIRED)

include_directories(${LIBNABLA_INCLUDE_DIRS})

set(NABLA_APP_SOURCES
    ${LIBNABLA_LIBRARIES}
)

add_executable(nabla
        main.cpp
)

target_link_libraries(nabla
    PRIVATE 
        ${NABLA_APP_SOURCES}
)