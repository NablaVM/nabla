/*
    Josh Bosley

    This is the main solace compiler entry program. It takes a file path on input and will feed the 
    solace compiler. As development continues it will be expanded with options of what to do with the 
    resulting output. It could start-up a vm and begin the execution, or it could dump the program to
    a file to be executed later.
*/

#include "assembler.hpp"
#include <fstream>
#include <iostream>

namespace
{
// I really don't want to go through all of the assembler code to change the debug statements to be macro wrapped,
// so we're doing it here. Speed really isn't an issue so its okay for now. Maybe change this if boredom
// comes and we want something to do
#ifdef NABLA_SOLACE_DEBUG_OUTPUT
    constexpr bool DEFAULT_VERBOSE = true;
#else
    constexpr bool DEFAULT_VERBOSE = false;
#endif
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

    if(!ASSEMBLER::ParseAsm(argv[1], bytes, DEFAULT_VERBOSE))
    {
        std::cerr << "Failed to parse asm. Goodbye." << std::endl;
        return 1;
    }

    std::ofstream out("solace.out", std::ios::out | std::ios::binary);
    if(!out.is_open())
    {
        std::cerr << "Unable to open solace.out for writing" << std::endl;
        return 1;
    }

    out.write(reinterpret_cast<const char*>(&bytes[0]), bytes.size());
    out.close();

    if(DEFAULT_VERBOSE){ std::cout << "Complete. " << bytes.size() << " bytes were generated." << std::endl; }
    return 0;
}
