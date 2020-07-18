#include <iostream>
#include <string>
#include <vector>

#include "NablaExec.hpp"

namespace
{
    struct Args
    {
        std::string short_arg;
        std::string long_arg;
        std::string description;
    };

    std::vector<Args> NablaArguments;
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
    APP::NablaExec exec; 

    return exec.execFile(file);
}