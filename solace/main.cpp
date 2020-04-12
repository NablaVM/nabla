#include "solace.hpp"

#include <iostream>

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

    if(!SOLACE::ParseAsm(argv[1], bytes))
    {
        std::cerr << "Failed to parse asm. Goodbye." << std::endl;
        return 1;
    }

    std::cout << "Complete. " << bytes.size() << " bytes were generated." << std::endl;

    return 0;
}
