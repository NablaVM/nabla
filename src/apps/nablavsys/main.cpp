#include <iostream>

extern "C" 
{
    #include "util.h"
}
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
        default:
            std::cout << "Unhandled load return" << std::endl;
            break;
    }

    std::cout << "About to run " << std::endl;

    bool continueRunning = true;

    while(continueRunning)
    {
        switch(virtualMachine.step(1))
        {
            case NABLA::VSYS::ExecutionReturns::OKAY :
            {
                break;
            }
            case NABLA::VSYS::ExecutionReturns::ALL_EXECUTION_COMPLETE :
            {
                std::cout << "Complete" << std::endl;
                break;
            }
            case NABLA::VSYS::ExecutionReturns::INSTRUCTION_NOT_FOUND :
            {
                std::cerr << "Instruction not found" << std::endl;
                break;
            }
            case NABLA::VSYS::ExecutionReturns::UNKNOWN_INSTRUCTION :
            {
                std::cerr << "Unknown instruction" << std::endl;
                break;
            }
            case NABLA::VSYS::ExecutionReturns::FAILED_TO_SPAWN_EXECUTION_CONTEXT :
            {
                std::cerr << "Failed to spawn new execution context" << std::endl;
                break;
            }
            case NABLA::VSYS::ExecutionReturns::EXECUTION_ERROR :
            {
                std::cerr << "Exectuion error" << std::endl;
                break;
            }
        }

        // Add a timer and run garbage collection
    }

    return 0;
}