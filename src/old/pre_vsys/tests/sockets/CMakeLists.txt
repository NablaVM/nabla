################################################
#   Solace
################################################

link_directories(${CPPUTEST_LIBRARIES})

include_directories(../)

include_directories(${NABLA_LIBC_INCLUDES})

add_executable(nablaSocketTests
        ${NABLA_LIBC_SOURCES}
        sockpoolTests.cpp
        main.cpp
)

target_link_libraries(nablaSocketTests 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaSocketTests COMMAND ./nablaSocketTests POST_BUILD)