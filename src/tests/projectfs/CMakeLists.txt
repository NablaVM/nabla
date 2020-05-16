################################################
#   
################################################

link_directories(${CPPUTEST_LIBRARIES})

include_directories(../)

include_directories(${NABLA_LIBC_INCLUDES})
include_directories(${NABLA_LIBCPP_INCLUDES})

add_executable(nablaProjectFsTests
        ${NABLA_LIBC_SOURCES}
        ${NABLA_LIBCPP_SOURCES}
        projectfsTests.cpp
        main.cpp
)

target_link_libraries(nablaProjectFsTests 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaProjectFsTests COMMAND ./nablaProjectFsTests POST_BUILD)