cmake_minimum_required(VERSION 3.8)

project(NablaApp)

option(COMPILE_TESTS        "Execute compile-time tests"     ON)

include(${CMAKE_SOURCE_DIR}/cmake/FindCppuTest.cmake)

#-------------------------------------------------
#   Nabla Version Information
#-------------------------------------------------

include(${CMAKE_SOURCE_DIR}/cmake/NablaVersion.cmake)

#-------------------------------------------------
#   Platform Information
#-------------------------------------------------

include(${CMAKE_SOURCE_DIR}/cmake/Platform.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-deprecated-register -Wno-unused-function")

find_package(libnabla REQUIRED)

include_directories(${LIBNABLA_INCLUDE_DIRS})


include_directories(${CMAKE_SOURCE_DIR}/exec)

set(EXEC_SRCS
        ${CMAKE_SOURCE_DIR}/exec/NablaExec.cpp
)


#-------------------------------------------------
#   Executable
#-------------------------------------------------

add_executable(nabla
        ${EXEC_SRCS}
        main.cpp
)

target_link_libraries(nabla
    PRIVATE 
        ${LIBNABLA_LIBRARIES}
)