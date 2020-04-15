/*
    Josh Bosley

    This is the main solace compiler entry program. It takes a file path on input and will feed the 
    solace compiler. As development continues it will be expanded with options of what to do with the 
    resulting output. It could start-up a vm and begin the execution, or it could dump the program to
    a file to be executed later.
*/

#include "solace.hpp"

#include <iostream>

namespace
{
    constexpr bool DEFAULT_VERBOSE = true;
}

int main(int argc, char ** argv)
{
    if(argc == 1)
    {
        std::cout << "No file name given" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(argc > 2)
    {
        std::cout << "Please only give one file" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<uint8_t> bytes;

    if(!SOLACE::ParseAsm(argv[1], bytes, DEFAULT_VERBOSE))
    {
        std::cerr << "Failed to parse asm. Goodbye." << std::endl;
        return 1;
    }

    std::cout << "Complete. " << bytes.size() << " bytes were generated." << std::endl;

    return 0;
}
