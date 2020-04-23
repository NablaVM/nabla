################################################
#   Solace
################################################

link_directories(${CPPUTEST_LIBRARIES})

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