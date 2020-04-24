################################################
#   NnetTests
################################################

link_directories(${CPPUTEST_LIBRARIES})

add_executable(nablaCompilerTests
        ParserTest.cpp
        main.cpp
)
target_link_libraries(nablaCompilerTests 
                      ${NABLA_COMPILER_SOURCES}
                      ${CPPUTEST_LDFLAGS})

add_custom_command(TARGET nablaCompilerTests COMMAND ./nablaCompilerTests POST_BUILD)