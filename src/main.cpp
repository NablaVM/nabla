#include <iostream>
#include <string>
#include <chrono>
#include <vector>

#include "CompilerFramework.hpp"
#include "InterpreterFramework.hpp"
#include "LibManifest.hpp"

#include <libnabla/projectfs.hpp>
#include <libnabla/VSysLoadableMachine.hpp>

/*
    LLL - Low level language    (LLL Binary is a binary generated from raw LLL through solace)
    HLL - High level language   (HLL Binary is a binary generated from nabla HLL compiler)
*/

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

    constexpr char LIB_LOCATION[] = "libs";
}

int handle_bin_exec(std::string file);

int handle_compilation(std::string file);

int handle_interpretation_cli();

int handle_interpretation_project(std::string project_dir);

void show_help();

void show_version();

// --------------------------------------------
// Entry
// --------------------------------------------
    
int main(int argc, char ** argv)
{
    if(argc == 1)
    {
        return handle_interpretation_cli();
    }

    NablaArguments = {

        { "-h", "--nabla-help", "Display help message."},
        { "-v", "--version",    "Display the version of Nabla." },
        { "-i", "--interpret",  "Interpret a Nabla HLL project."},
        { "-c", "--compile",    "Compile a nabla HLL project."}
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

        // Interpret a nabla HLL project (run without compile)
        //
        if(args[i] == "-i" || args[i] == "--interpret")
        {
            if(i == argc - 1)
            {
                std::cout << "Error: Project directory not given" << std::endl;
                return 1;
            }

            return handle_interpretation_project(args[i+1]);
        }
        
        // Compile a nabla HLL project
        //
        if(args[i] == "-c" || args[i] == "--compile")
        {
            if(i == argc - 1)
            {
                std::cout << "Error: Project directory not given" << std::endl;
                return 1;
            }

            return handle_compilation(args[i+1]);
        }
    }

    // No arguments handled, but there is at least one argument so try to execute it as a HLL project

    return handle_bin_exec(args[1]);
}

// --------------------------------------------
// Load a project
// --------------------------------------------

bool load_project(std::string project_dir, NABLA::ProjectFS & project)
{
    switch(project.load(project_dir))
    {
        case NABLA::ProjectFS::LoadResultCodes::OKAY:
            std::cout << "[" << project_dir << "] loaded!" << std::endl;
            return true;

        case NABLA::ProjectFS::LoadResultCodes::ERROR_GIVEN_PATH_NOT_DIRECTORY:
            std::cerr << "[" << project_dir << "] Is not a directory" << std::endl;
            return false;

        case NABLA::ProjectFS::LoadResultCodes::ERROR_FAILED_TO_OPEN_CONFIG:
            std::cerr << "Unable to open config.json" << std::endl;
            return false;

        case NABLA::ProjectFS::LoadResultCodes::ERROR_FAILED_TO_LOAD_CONFIG:
            std::cerr << "Unable to load config.json" << std::endl;
            return false;
    }
    return false;
}

// --------------------------------------------
// Compile Nabla HLL
// --------------------------------------------
    
int handle_compilation(std::string project_dir)
{
    std::cout << " ∇ Nabla ∇ " << NABLA_VERSION_INFO     << std::endl 
              << "Platform: "  << TARGET_PLATFORM_STRING << std::endl
              << "------------------------------------"  << std::endl; 

    NABLA::LibManifest lib_manifest;

    // Load the system library manifest. Errors reported by method call
    if(!lib_manifest.load_manifest(LIB_LOCATION))
    {
        return 1;
    }

    NABLA::ProjectFS project;

    if(!load_project(project_dir, project))
    {
        return 1;
    }

    NABLA::CompilerFramework cfw(lib_manifest);

    return cfw.compile_project(project);
}

// --------------------------------------------
// Interpret Nabla HLL cli
// --------------------------------------------

int handle_interpretation_cli()
{
    std::cout << " ∇ Nabla ∇ " << NABLA_VERSION_INFO     << std::endl 
              << "Platform: "  << TARGET_PLATFORM_STRING << std::endl
              << "------------------------------------"  << std::endl; 

    NABLA::LibManifest lib_manifest;

    // Load the system library manifest. Errors reported by method call
    if(!lib_manifest.load_manifest(LIB_LOCATION))
    {
        return 1;
    }

    NABLA::InterpreterFramework interpfw(lib_manifest);

    return interpfw.interpret_cli();
}

// --------------------------------------------
// Interpret Nabla HLL Project
// --------------------------------------------

int handle_interpretation_project(std::string project_dir)
{
    std::cout << " ∇ Nabla ∇ " << NABLA_VERSION_INFO     << std::endl 
              << "Platform: "  << TARGET_PLATFORM_STRING << std::endl
              << "------------------------------------"  << std::endl; 

    NABLA::LibManifest lib_manifest;

    // Load the system library manifest. Errors reported by method call
    if(!lib_manifest.load_manifest(LIB_LOCATION))
    {
        return 1;
    }

    std::cerr << "Project Interpreter has not yet been completed" << std::endl;

    NABLA::ProjectFS project;

    if(!load_project(project_dir, project))
    {
        return 1;
    }

    NABLA::InterpreterFramework interpfw(lib_manifest);

    return interpfw.interpret_project(project);
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

    std::cout << "----------------------------------------------"
              << std::endl 
              << "Given no commands, Nabla will enter into interpreter."
              << std::endl 
              << "Given a single file, Nabla will assume that it is a bytecode file and attempt to execute it."
              << std::endl;
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