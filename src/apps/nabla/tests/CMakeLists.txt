################################################
#   Nabla Tests
################################################

link_directories(${CPPUTEST_LIBRARIES})

include_directories(../)
include_directories(${CMAKE_CURRENT_LIST_DIR})
include_directories(${CMAKE_SOURCE_DIR}/libcpp/bytegen/)
include_directories(${CMAKE_SOURCE_DIR}/libc/binloader)
include_directories(${CMAKE_SOURCE_DIR}/libc/stack)
include_directories(${CMAKE_SOURCE_DIR}/libc/vm)

add_executable(nablaVmTests
        ${NABLA_APP_SOURCES}
        ${CMAKE_SOURCE_DIR}/libcpp/bytegen/bytegen.cpp
        baseInstructionTests.cpp
        main.cpp
)

target_link_libraries(nablaVmTests 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaVmTests COMMAND ./nablaVmTests POST_BUILD)