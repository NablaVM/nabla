################################################
#   Solace
################################################

if(DEFINED ENV{CPPUTEST_HOME})
    message(STATUS "Using CppUTest home: $ENV{CPPUTEST_HOME}")
    set(CPPUTEST_INCLUDE_DIRS $ENV{CPPUTEST_HOME}/include)
    set(CPPUTEST_LIBRARIES $ENV{CPPUTEST_HOME}/lib)
    set(CPPUTEST_LDFLAGS CppUTest CppUTestExt)
else()
    find_package(PkgConfig REQUIRED)
    pkg_search_module(CPPUTEST REQUIRED cpputest>=3.8)
    message(STATUS "Found CppUTest version ${CPPUTEST_VERSION}")
endif()

link_directories(${CPPUTEST_LIBRARIES})

message(${CMAKE_SOURCE_DIR})

include_directories(../)
include_directories(${CMAKE_SOURCE_DIR}/libc/vm)
include_directories(${CMAKE_SOURCE_DIR}/libcpp/bytegen)

add_executable(nablaInsTest
        ${SOLACE_COMPILER_SOURCES}
        arithmaticTests.cpp
        branchTests.cpp
        constantTests.cpp
        callTests.cpp
        exitTests.cpp
        jumpTests.cpp
        movTests.cpp
        pushpopTests.cpp
        returnTests.cpp
        stbldbTests.cpp
        main.cpp
)

target_link_libraries(nablaInsTest 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaInsTest COMMAND ./nablaInsTest POST_BUILD)