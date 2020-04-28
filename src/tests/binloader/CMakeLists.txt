################################################
#   Solace
################################################

link_directories(${CPPUTEST_LIBRARIES})

include_directories(../)

include_directories(${NABLA_LIBC_INCLUDES})
include_directories(${NABLA_LIBCPP_INCLUDES})
include_directories(${CMAKE_SOURCE_DIR}/apps/solace)

add_executable(nablaBinloaderTests
        ${NABLA_LIBC_SOURCES}
        ${NABLA_LIBCPP_SOURCES}
        ${CMAKE_SOURCE_DIR}/apps/solace/solace.cpp
        binloaderTests.cpp
        main.cpp
)

target_link_libraries(nablaBinloaderTests 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaBinloaderTests COMMAND ./nablaBinloaderTests POST_BUILD)