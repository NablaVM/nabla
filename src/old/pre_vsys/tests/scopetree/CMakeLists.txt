################################################
#   
################################################

link_directories(${CPPUTEST_LIBRARIES})

include_directories(../)

include_directories(${NABLA_LIBC_INCLUDES})
include_directories(${NABLA_LIBCPP_INCLUDES})

add_executable(nablaScopeTreeTests
        ${NABLA_LIBC_SOURCES}
        ${NABLA_LIBCPP_SOURCES}
        scopeTreeTests.cpp
        main.cpp
)

target_link_libraries(nablaScopeTreeTests 
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaScopeTreeTests COMMAND ./nablaScopeTreeTests POST_BUILD)