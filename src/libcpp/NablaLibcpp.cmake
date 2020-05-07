
set(NABLA_LIBCPP_INCLUDES
    ${CMAKE_SOURCE_DIR}/libcpp/assembler
    ${CMAKE_SOURCE_DIR}/libcpp/bytegen
    ${CMAKE_SOURCE_DIR}/libcpp/interpreter
)

set(NABLA_LIBCPP_HEADERS
    ${CMAKE_SOURCE_DIR}/libcpp/assembler/assembler.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/bytegen/bytegen.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/interpreter/interpreter.hpp
)

set(NABLA_LIBCPP_SOURCES
    ${CMAKE_SOURCE_DIR}/libcpp/assembler/assembler.cpp
    ${CMAKE_SOURCE_DIR}/libcpp/bytegen/bytegen.cpp
    ${CMAKE_SOURCE_DIR}/libcpp/interpreter/interpreter.cpp
)