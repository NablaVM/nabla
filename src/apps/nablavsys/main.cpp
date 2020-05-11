#include <iostream>

#include "VSysMachine.hpp"          // raw virtual machine
#include "VSysLoadableMachine.hpp"  // Binary-loadable machine


int main(int argc, char **argv)
{

    std::cout << "Nabla with new vsys architecture" << std::endl;

    NABLA::VSYS::LoadableMachine virtualMachine;

    switch(virtualMachine.loadFile("solace.out"))
    {
        case NABLA::VSYS::LoadableMachine::LoadResultCodes::OKAY :
        {
            std::cout << "VSYS loaded" << std::endl;
            break;
        }
        case NABLA::VSYS::LoadableMachine::LoadResultCodes::UNABLE_TO_OPEN_FILE :
        {
            std::cout << "Unable to open file!" << std::endl;
            break;
        }
        case NABLA::VSYS::LoadableMachine::LoadResultCodes::ERROR_FAILED_TO_LOAD_CONSTANTS :
        {
            std::cout << "Failed to load constants" << std::endl;
            break;
        }
        case NABLA::VSYS::LoadableMachine::LoadResultCodes::ERROR_FAILED_TO_LOAD_FUCNTION :
        {
            std::cout << "Failed to load function" << std::endl;
            break;
        }
        case NABLA::VSYS::LoadableMachine::LoadResultCodes::ERROR_UNHANDLED_INSTRUCTION :
        {
            std::cout << "Unhandled instruction" << std::endl;
            break;
        }
        case NABLA::VSYS::LoadableMachine::LoadResultCodes::ERROR_EOB_NOT_FOUND :
        {
            std::cout << "EOB not found" << std::endl;
            break;
        }
    }

    return 0;
}