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

find_package(Threads)
find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

include_directories(${LIBNABLA_INCLUDE_DIRS})

#-------------------------------------------------
#   Compilers
#-------------------------------------------------

set(NABLA_COMPILERS_DIR 
    ${CMAKE_CURRENT_SOURCE_DIR}/compilers
)

include(${NABLA_COMPILERS_DIR}/Compilers.cmake)

include_directories(
    ${NABLA_COMPILERS_INCLUDE_DIRS}
)

#-------------------------------------------------
#   Framework
#-------------------------------------------------

set(FRAMEWORK_SOURCES
    ${CMAKE_SOURCE_DIR}/framework/CompilerFramework.cpp
    ${CMAKE_SOURCE_DIR}/framework/LibManifest.cpp
)

include_directories(
    ${CMAKE_SOURCE_DIR}/framework
)

#-------------------------------------------------
#   App 
#-------------------------------------------------

set(NABLA_APP_SOURCES
    ${LIBNABLA_LIBRARIES}
)

#-------------------------------------------------
#   Executable
#-------------------------------------------------

add_executable(nabla
        main.cpp
        ${FRAMEWORK_SOURCES}
        ${NABLA_COMPILERS_SOURCES}
)

target_link_libraries(nabla
    PRIVATE 
        ${NABLA_APP_SOURCES}
        Threads::Threads
)

#-------------------------------------------------
#   Executable
#-------------------------------------------------

add_custom_target(copy-nabla-examples ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/examples ${CMAKE_BINARY_DIR}/examples
)

add_custom_target(copy-nabla-stdlib ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/libs ${CMAKE_BINARY_DIR}/libs
)
