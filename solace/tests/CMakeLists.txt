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

add_executable(nablaCompilerTests
        ParserTest.cpp
        main.cpp
)
target_link_libraries(nablaCompilerTests 
                      ${NABLA_COMPILER_SOURCES}
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaCompilerTests COMMAND ./nablaCompilerTests POST_BUILD)