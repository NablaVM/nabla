################################################
#   Solace
################################################

link_directories(${CPPUTEST_LIBRARIES})

include_directories(../)

include_directories(${NABLA_LIBC_INCLUDES})

add_executable(nablaStackTests
        ${NABLA_LIBC_SOURCES}
        stackTests.cpp
        main.cpp
)

target_link_libraries(nablaStackTests 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaStackTests COMMAND ./nablaStackTests POST_BUILD)