#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <vector>
#include <cstdint>
#include <sstream>
#include <iterator>
#include <bitset>
#include <iomanip>
#include "Analyzer.hpp"

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

void analyzeFile(std::string file);


// ---------------------------------------------------------------
//
// --------------------------------------------------------------- 

int main(int argc, char ** argv)
{
    std::vector<std::string> arguments(argv+1, argv+argc);

    bool analyze   = false;
    bool decompile = false;
    bool fileGiven = false;
    std::string fileName;
    for(int i = 0; i < arguments.size(); i++)
    {
        if(arguments[i] == "--analyze"   || arguments[i] == "-a") { analyze   = true; }
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

    if(decompile && analyze)
    {
        // This might be something we want to do though?
        std::cerr << "Can only view OR decompile. Not both" << std::endl;
        return 1;
    }

    if(analyze)
    {
        analyzeFile(fileName);
    }

    if(decompile)
    {
        std::cout << TERM_RED << "Decompiler not yet completed!" << TERM_RESET << std::endl;
    }

    return 0;
}

// ---------------------------------------------------------------
//
// --------------------------------------------------------------- 

void show64Hex(uint64_t value)
{
    std::cout << "\t" 
                << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((value & 0xFF00000000000000) >> 56 ) << " | "
                << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((value & 0x00FF000000000000) >> 48 ) << " | "
                << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((value & 0x0000FF0000000000) >> 40 ) << " | "
                << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((value & 0x000000FF00000000) >> 32 ) << " | "
                << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((value & 0x00000000FF000000) >> 24 ) << " | "
                << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((value & 0x0000000000FF0000) >> 16 ) << " | "
                << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((value & 0x000000000000FF00) >> 8  ) << " | "
                << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << ((value & 0x00000000000000FF) >> 0  )
                << std::endl;
}

// ---------------------------------------------------------------
//
// --------------------------------------------------------------- 

void show64Bin(uint64_t value)
{
    std::cout << "\t" 
                << std::bitset<8>((value & 0xFF00000000000000) >> 56 ) << " | "
                << std::bitset<8>((value & 0x00FF000000000000) >> 48 ) << " | "
                << std::bitset<8>((value & 0x0000FF0000000000) >> 40 ) << " | "
                << std::bitset<8>((value & 0x000000FF00000000) >> 32 ) << " | "
                << std::bitset<8>((value & 0x00000000FF000000) >> 24 ) << " | "
                << std::bitset<8>((value & 0x0000000000FF0000) >> 16 ) << " | "
                << std::bitset<8>((value & 0x000000000000FF00) >> 8  ) << " | "
                << std::bitset<8>((value & 0x00000000000000FF) >> 0  )
                << std::endl;
}

// ---------------------------------------------------------------
//
// --------------------------------------------------------------- 

void analyzeFile(std::string fileName)
{
    HARP::Analyzer * analyzer = new HARP::Analyzer();

    if(!analyzer->loadBin(fileName))
    {
        std::cerr << "Error analyzing : " << fileName << std::endl;
        exit(EXIT_FAILURE);
    }
    std::vector<HARP::Analyzer::FunctionInfo> functions = analyzer->getFunctions();

    std::cout << TERM_GREEN << "Used 'exit' to exit, and 'help' for help" << TERM_RESET << std::endl;

    std::string input;

    std::cout << "harp> ";
    while(getline(std::cin, input))
    {
        if(input.size() == 0)
        {
            goto skip_input;
        }

        if(input == "exit")
        {
            return;
        }
        else if(input == "help" || input == "h")
        {
            std::cout << "\texit     ......................... exits analyzer"                  << std::endl
                      << "\thelp | h ......................... show this message"               << std::endl
                      << "\tgs       ......................... global stack"                    << std::endl
                      << "\tcs       ......................... call stack"                      << std::endl
                      << "\tregs     ......................... registers"                       << std::endl
                      << "\tfc       ......................... function count"                  << std::endl
                      << "\tfp       ......................... function pointer"                << std::endl
                      << "\tfd       ......................... function dump (-b for binary)"   << std::endl
                      << "\tea       ......................... show entry address"              << std::endl
                      << "\tstep | s ......................... step vm execution of bin 1 step" << std::endl
                      << "\treset    ......................... reset and reload current bin"    << std::endl;
        }
        else
        {
            std::istringstream iss(input);
            std::vector<std::string> results((std::istream_iterator<std::string>(iss)),
                                            std::istream_iterator<std::string>());

            if(results[0] == "gs")
            {
                std::vector<uint64_t> gs   = analyzer->getGlobalStack();
                for(uint64_t i = 0; i < gs.size(); i++)
                {
                    std::cout << TERM_CYAN << "GS Address " << TERM_RESET << TERM_YELLOW << "0x" << std::hex << std::uppercase 
                              << std::setfill('0') << std::setw(8) << (int)i << TERM_RESET << std::endl << std::endl;
                    show64Hex(gs[i]);
                }
                std::cout << std::endl;

            }
            else if(results[0] == "cs")
            {
                std::vector<uint64_t> cs   = analyzer->getCallStack();
                for(uint64_t i = 0; i < cs.size(); i++)
                {
                    std::cout << std::endl << TERM_CYAN << "CS Address " << TERM_RESET << TERM_YELLOW << "0x" << std::hex << std::uppercase 
                              << std::setfill('0') << std::setw(8) << (int)i << TERM_RESET << std::endl << std::endl;
                    show64Hex(cs[i]);
                }
                std::cout << std::endl;
            }
            else if(results[0] == "regs")
            {
                std::vector<int64_t>  regs = analyzer->getRegisters();
                for(uint8_t i = 0; i < regs.size(); i++)
                {
                    std::cout << TERM_CYAN << "REG " << TERM_RESET << TERM_YELLOW << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (int)i << TERM_RESET;
                    show64Hex(regs[i]);
                }
                std::cout << std::endl;
            }
            else if(results[0] == "fc")
            {
                std::cout << TERM_CYAN << "Function Count :" << TERM_RESET << " " << functions.size() << std::endl << std::endl;
            }
            else if(results[0] == "fp")
            {
                std::cout << TERM_CYAN << "Function Pointer :" << TERM_RESET << " " << analyzer->getFunctionPointer() << std::endl;
            }
            else if(results[0] == "fd")
            {
                bool doBin = false;
                if(results.size() > 1)
                {
                    if(results[1] == "-b")
                    {
                        doBin = true;
                    }
                    else
                    {
                        std::cout << TERM_RED << "Unknown flag: " << TERM_RESET << results[1] << std::endl;
                        goto skip_input;
                    }
                }

                for(uint64_t fidx = 0; fidx < functions.size(); fidx++)
                {
                    std::cout << TERM_CYAN << "Address " << TERM_RESET << TERM_YELLOW 
                              << "0x" << std::hex << std::uppercase << std::setfill('0') 
                              << std::setw(8) << functions[fidx].address << TERM_RESET << std::endl << std::endl;

                    for(auto & i : functions[fidx].instructions)
                    {
                        (doBin) ?  show64Bin(i) : show64Hex(i);
                    }
                    std::cout << std::endl;
                }
            }
            else if(results[0] == "ea")
            {
                std::cout << TERM_CYAN << "Entry Address (main) : " << TERM_RESET 
                          << TERM_YELLOW << "0x" << std::hex << std::uppercase 
                          << std::setfill('0') << std::setw(8) <<  analyzer->getEntryAddress() << TERM_RESET << std::endl << std::endl;
            }
            else if(results[0] == "step" || results[0] == "s")
            {
                analyzer->step(1);
            }
            else if(results[0] == "reset")
            {
                delete analyzer;
                analyzer = new HARP::Analyzer();

                if(!analyzer->loadBin(fileName))
                {
                    std::cerr << "Error analyzing : " << fileName << std::endl;
                    exit(EXIT_FAILURE);
                }
                functions = analyzer->getFunctions();
            }
        }

skip_input:
        // End
        std::cout << "harp> ";
    }


}