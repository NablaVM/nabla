################################################
#   
################################################

link_directories(${CPPUTEST_LIBRARIES})

include_directories(../)

include_directories(${NABLA_LIBCPP_INCLUDES})
include_directories(${NABLA_LIBC_INCLUDES})

add_executable(nablaVsysTests
        ${NABLA_LIBCPP_SOURCES}
        ${NABLA_LIBC_SOURCES}
        arithTests.cpp
        memory.cpp
        main.cpp
)

target_link_libraries(nablaVsysTests 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaVsysTests COMMAND ./nablaVsysTests POST_BUILD)