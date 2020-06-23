#include <iostream>
#include <string>
#include <chrono>
#include <vector>

#include <libnabla/VSysLoadableMachine.hpp>

namespace
{
    struct Args
    {
        std::string short_arg;
        std::string long_arg;
        std::string description;
    };

    std::vector<Args> NablaArguments;

    constexpr double SECONDS_BETWEEN_GC_CYCLES = 30.0;
}

int handle_bin_exec(std::string file);

void show_help();

void show_version();

// --------------------------------------------
// Entry
// --------------------------------------------
    
int main(int argc, char ** argv)
{
    if(argc == 1)
    {
        std::cout << "No input given. Use -h for help" << std::endl;
        return 0;
    }

    NablaArguments = {

        { "-h", "--nabla-help", "Display help message."},
        { "-v", "--version",    "Display the version of Nabla." }
    };
    
    std::vector<std::string> args(argv, argv + argc);

    for(int i = 0; i < argc; i++)
    {
        //  Help
        //
        if(args[i] == "-h" || args[i] == "--nabla-help")
        {
            show_help();
            return 0;
        }

        // Version info
        //
        if(args[i] == "-v" || args[i] == "--version")
        {
            show_version();
            return 0;
        }
    }

    // No arguments handled, but there is at least one argument so try to execute it 
    return handle_bin_exec(args[1]);
}

// --------------------------------------------
// Show help
// --------------------------------------------
    
void show_help()
{
    std::cout << " ∇ Nabla ∇ | Help" << std::endl 
              << "----------------------------------------------" 
              << std::endl
              << "Short\tLong\t\tDescription" << std::endl
              << "----------------------------------------------"
              << std::endl;

    for(auto & na : NablaArguments)
    {
        std::cout << na.short_arg << "\t" << na.long_arg << "\t" << na.description << std::endl;
    }

    std::cout << std::endl << "To start the VM pass in a binary file as an argument to the Nabla application" << std::endl;
}

// --------------------------------------------
// Show version
// --------------------------------------------
    
void show_version()
{
    std::cout << " ∇ Nabla ∇ | Version and build information"       << std::endl
              << "-------------------------------------------"      << std::endl 
              << "Nabla Version  : " << NABLA_VERSION_INFO          << std::endl
              << "-------------------------------------------"      << std::endl;
    return;
}

// --------------------------------------------
//  Execute a binary 
// --------------------------------------------

int  handle_bin_exec(std::string file)
{
    NABLA::VSYS::LoadableMachine virtualMachine;

    switch(virtualMachine.loadFile(file))
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
        if(!virtualMachine.isRunning())
        {
            continueRunning = false;
        }

        // Timered context garbage collection
        std::chrono::duration<double> elapsed_seconds = std::chrono::steady_clock::now() - start;

        if(SECONDS_BETWEEN_GC_CYCLES < elapsed_seconds.count())
        {
            virtualMachine.executionContextGarbageCollection();

            start = std::chrono::steady_clock::now();
        }
    }

    return 0;
}