
set(NABLA_LIBCPP_INCLUDES
    ${CMAKE_SOURCE_DIR}/libcpp/assembler
    ${CMAKE_SOURCE_DIR}/libcpp/bytegen
    ${CMAKE_SOURCE_DIR}/libcpp/scopetree


    ${CMAKE_SOURCE_DIR}/libcpp/vsys
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/hardware

)

set(NABLA_LIBCPP_HEADERS
    ${CMAKE_SOURCE_DIR}/libcpp/assembler/assembler.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/bytegen/bytegen.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/scopetree/scopetree.hpp


    ${CMAKE_SOURCE_DIR}/libcpp/vsys/VSysExecutionContext.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/VSysExecutionReturns.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/VSysMachine.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/VSysMemory.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/VSysSettings.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/VSysLoadableMachine.hpp


    ${CMAKE_SOURCE_DIR}/libcpp/vsys/hardware/VSysExternalIO.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/hardware/VSysExternalNet.hpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/hardware/VSysExternalHost.hpp
)

set(NABLA_LIBCPP_SOURCES
    ${CMAKE_SOURCE_DIR}/libcpp/assembler/assembler.cpp
    ${CMAKE_SOURCE_DIR}/libcpp/bytegen/bytegen.cpp
    ${CMAKE_SOURCE_DIR}/libcpp/scopetree/scopetree.cpp


    ${CMAKE_SOURCE_DIR}/libcpp/vsys/VSysExecutionContext.cpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/VSysMachine.cpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/VSysLoadableMachine.cpp


    ${CMAKE_SOURCE_DIR}/libcpp/vsys/hardware/VSysExternalIO.cpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/hardware/VSysExternalNet.cpp
    ${CMAKE_SOURCE_DIR}/libcpp/vsys/hardware/VSysExternalHost.cpp
)