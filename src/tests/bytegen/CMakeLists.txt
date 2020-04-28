################################################
#   Solace
################################################

link_directories(${CPPUTEST_LIBRARIES})

include_directories(../)

include_directories(${NABLA_LIBC_INCLUDES})
include_directories(${NABLA_LIBCPP_INCLUDES})

add_executable(nablaBytegenTests
        ${NABLA_LIBCPP_SOURCES}
        arithmaticTests.cpp
        bitwiseTests.cpp
        branchTests.cpp
        constantTests.cpp
        callTests.cpp
        exitTests.cpp
        jumpTests.cpp
        movTests.cpp
        nopTests.cpp
        pushpopTests.cpp
        returnTests.cpp
        sizeTests.cpp
        stbldbTests.cpp
        main.cpp
)

target_link_libraries(nablaBytegenTests 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaBytegenTests COMMAND ./nablaBytegenTests POST_BUILD)