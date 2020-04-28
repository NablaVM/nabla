################################################
#   Solace
################################################

link_directories(${CPPUTEST_LIBRARIES})

include_directories(../)

include_directories(${NABLA_LIBC_INCLUDES})

add_executable(nablaUtilityTests
        ${NABLA_LIBC_SOURCES}
        utilTests.cpp
        main.cpp
)

target_link_libraries(nablaUtilityTests 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaUtilityTests COMMAND ./nablaUtilityTests POST_BUILD)