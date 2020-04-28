################################################
#   Nabla Tests
################################################

link_directories(${CPPUTEST_LIBRARIES})

include_directories(../)
include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${NABLA_LIBCPP_INCLUDES})
include_directories(${NABLA_LIBC_INCLUDES})

add_executable(nablaVmTests
        ${NABLA_LIBC_SOURCES}
        ${NABLA_LIBCPP_SOURCES}
        instructionExecutionTests.cpp
        main.cpp
)

target_link_libraries(nablaVmTests 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaVmTests COMMAND ./nablaVmTests POST_BUILD)