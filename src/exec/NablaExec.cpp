#include "NablaExec.hpp"

#include <iostream>
#include <chrono>

namespace APP
{
    NablaExec::NablaExec(double gc_cycle_sec) : gc_cycle(gc_cycle_sec),
                                                virtualMachine(nullptr)
    {
        
    }

    NablaExec::~NablaExec()
    {
        if(nullptr != virtualMachine)
        {
            delete virtualMachine;
        }
    }

    int NablaExec::execFile(std::string file)
    {   
        if(virtualMachine)
        {
            if(virtualMachine->isRunning())
            {
                std::cerr << "NabalExec instance's VM is still running" << std::endl;
                return 1;
            }
            else
            {
                delete virtualMachine;
                virtualMachine = nullptr;
            }
        }

        virtualMachine = new NABLA::VSYS::LoadableMachine();

        switch(virtualMachine->loadFile(file))
        {
            case NABLA::VSYS::LoadableMachine::LoadResultCodes::OKAY :
            {
                break;
            }
            case NABLA::VSYS::LoadableMachine::LoadResultCodes::ERROR_MACHINE_IN_ERROR_STATE:
            {
                std::cerr << "Machine was in error state when attempting to load file" << std::endl;
                return 1;
            }
            case NABLA::VSYS::LoadableMachine::LoadResultCodes::UNABLE_TO_OPEN_FILE :
            {
                std::cerr << "Unable to open file!" << std::endl;
                return 1;
            }
            case NABLA::VSYS::LoadableMachine::LoadResultCodes::ERROR_FAILED_TO_LOAD_CONSTANTS :
            {
                std::cerr << "Failed to load constants" << std::endl;
                return 1;
            }
            case NABLA::VSYS::LoadableMachine::LoadResultCodes::ERROR_FAILED_TO_LOAD_FUCNTION :
            {
                std::cerr << "Failed to load function" << std::endl;
                return 1;
            }
            case NABLA::VSYS::LoadableMachine::LoadResultCodes::ERROR_UNHANDLED_INSTRUCTION :
            {
                std::cerr << "Unhandled instruction" << std::endl;
                return 1;
            }
            case NABLA::VSYS::LoadableMachine::LoadResultCodes::ERROR_EOB_NOT_FOUND :
            {
                std::cerr << "EOB not found" << std::endl;
                return 1;
            }
            default:
                std::cerr << "Unhandled load return" << std::endl;
                return 1;
        }

        bool continueRunning = true;

        auto start = std::chrono::steady_clock::now();

        while(continueRunning)
        {
            switch(virtualMachine->step(1))
            {
                case NABLA::VSYS::ExecutionReturns::OKAY :
                {
                    break;
                }
                case NABLA::VSYS::ExecutionReturns::ALL_EXECUTION_COMPLETE :
                {
                    std::cout << "Complete" << std::endl;

                    delete virtualMachine;
                    virtualMachine = nullptr;
                    return 0;
                }
                case NABLA::VSYS::ExecutionReturns::INSTRUCTION_NOT_FOUND :
                {
                    std::cerr << "Instruction not found" << std::endl;
                    return 1;
                }
                case NABLA::VSYS::ExecutionReturns::UNKNOWN_INSTRUCTION :
                {
                    std::cerr << "Unknown instruction" << std::endl;
                    return 1;
                }
                case NABLA::VSYS::ExecutionReturns::FAILED_TO_SPAWN_EXECUTION_CONTEXT :
                {
                    std::cerr << "Failed to spawn new execution context" << std::endl;
                    return 1;
                }
                case NABLA::VSYS::ExecutionReturns::EXECUTION_ERROR :
                {
                    std::cerr << "Exectuion error" << std::endl;
                    return 1;
                }
            }

            // If VM has stopped. We should stop
            if(!virtualMachine->isRunning())
            {
                continueRunning = false;
            }

            // Timered context garbage collection
            std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;

            if(gc_cycle < elapsed_seconds.count())
            {
                virtualMachine->executionContextGarbageCollection();

                start = std::chrono::steady_clock::now();
            }
        }

        return 0;
    }
}