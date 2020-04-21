#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstdint>

#include "InsManifest.hpp"

#define TERM_RESET       "\033[0m"
#define TERM_BLACK       "\033[30m"              // Black
#define TERM_RED         "\033[31m"              // Red
#define TERM_GREEN       "\033[32m"              // Green
#define TERM_YELLOW      "\033[33m"              // Yellow
#define TERM_BLUE        "\033[34m"              // Blue
#define TERM_MAGENTA     "\033[35m"              // Magenta
#define TERM_CYAN        "\033[36m"              // Cyan
#define TERM_WHITE       "\033[37m"              // White
#define TERM_BOLDBLACK   "\033[1m\033[30m"       // Bold Black 
#define TERM_BOLDRED     "\033[1m\033[31m"       // Bold Red 
#define TERM_BOLDGREEN   "\033[1m\033[32m"       // Bold Green 
#define TERM_BOLDYELLOW  "\033[1m\033[33m"       // Bold Yellow 
#define TERM_BOLDBLUE    "\033[1m\033[34m"       // Bold Blue 
#define TERM_BOLDMAGENTA "\033[1m\033[35m"       // Bold Magenta 
#define TERM_BOLDCYAN    "\033[1m\033[36m"       // Bold Cyan 
#define TERM_BOLDWHITE   "\033[1m\033[37m"       // Bold White 

std::vector<uint8_t> readFile(std::string file);

void decompileFile(std::vector<uint8_t> bytes);

void viewFile(std::vector<uint8_t> bytes);

// ---------------------------------------------------------------
//
// --------------------------------------------------------------- 

int main(int argc, char ** argv)
{
    std::vector<std::string> arguments(argv+1, argv+argc);

    bool view = false;
    bool decompile = false;
    bool fileGiven = false;
    std::string fileName;
    for(int i = 0; i < arguments.size(); i++)
    {
        if(arguments[i] == "--view"      || arguments[i] == "-v") { view = true;      }
        if(arguments[i] == "--decompile" || arguments[i] == "-d") { decompile = true; }

        if(arguments[i] == "--file" || arguments[i] == "-f")
        {
            if(fileGiven)
            {
                std::cerr << "File already given as : " << fileName << std::endl;
                return 1;
            }

            if(i+1 == arguments.size())
            {
                std::cerr << "No file name given after --file (-f)" << std::endl;
                return 1;
            }

            fileName = arguments[i+1];
            fileGiven = true;
        }
    }

    if(!fileGiven)
    {
        std::cerr << "No file given" << std::endl;
        return 1;
    }

    if(decompile && view)
    {
        // This might be something we want to do though?
        std::cerr << "Can only view OR decompile. Not both" << std::endl;
        return 1;
    }

    std::vector<uint8_t>  bytes = readFile(fileName);

    if(decompile)
    {
        decompileFile(bytes);
    }

    if(view)
    {
        viewFile(bytes);
    }

    return 0;
}

// ---------------------------------------------------------------
//
// --------------------------------------------------------------- 

std::vector<uint8_t> readFile(std::string file)
{
    std::vector<uint8_t> bytes;

    std::ifstream f(file, std::ios::in | std::ios::binary);

    if(!f.is_open())
    {
        std::cerr << "Unable to open : " << file << std::endl;
        exit(EXIT_FAILURE);
    }

    char currentByte;
    while(f.read(&currentByte, sizeof(uint8_t)))
    {
        bytes.push_back(static_cast<uint8_t>(currentByte));
    }

    f.close();

    return bytes;
}

// ---------------------------------------------------------------
//
// --------------------------------------------------------------- 

void decompileFile(std::vector<uint8_t> bytes)
{
    std::cout << "Decompile : " << bytes.size() << " bytes" << std::endl;
}

// ---------------------------------------------------------------
//
// --------------------------------------------------------------- 

void viewFile(std::vector<uint8_t> bytes)
{
    std::cout << "View : " << bytes.size() << " bytes" << std::endl;

    std::vector<uint8_t> insVec = HARP::MANIFEST::GetInsVec();


    int line = 0;
    int byte = 0;

    std::string padding = " ";

    for(int i = 0; i < bytes.size(); i++)
    {
        if(bytes[i] == CONS)


        auto it = std::find (insVec.begin(), insVec.end(), b);
        if(it != insVec.end())
        {
            std::cout << TERM_BOLDGREEN << std::left << "0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (int)b << TERM_RESET << " ";
        }
        else
        {
            std::cout << std::left << "0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << (int)b << " ";
        }

        byte ++;

        if(byte % 8 == 0)
        {
            std::cout << std::endl;
        }
    }

}