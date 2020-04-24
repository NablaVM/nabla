/*
    Josh. A Bosley


    Planned updates:
            Things that I would like to do in the future for specific types have a large comment in them
            describing the update along with a 'DEVELOPMENT_NOTE' tag so it can be found easily.
            Once solace and byte gen get working these should be put onto a project board


    This parser is a bit long and in need of explanation. The basic gist is this :

        A file name is given to parse. We open the file and iterate over each line.

        On the first pass labels, and functions are enumerated for addressing while the lines are split apart by relevant chunks of information.
        Blank lines are discarded. Relevant chunks are any space seperated groups of chars with the exception of declared strings, which are preserved.

        For instance :    
                            this is an example "of something" that gets chunked
        Will be stored as :
                            [this, is, an, example, "of something", that, gets, chunked]
        
        On the second pass we  we iterate over the chunked lines. We go through a vector (parserMethods) that matches regex against the 
        0th element of the chunk list in an attempt to locate an instruction. 
        
        If a known instruction is matched, its corresponding function will be called as layed-out by the 'parserMethods' vector. 

        If no expression is matched, the line is deemed invalid and we indicate this by returning false and refusing to continue parsing
    
        A matched expression's call will handle the particulars of instruction. It will validate the arguments allowed by the instruction and 
        then call into the bytegenerator to get create the corresponding byte code for the instruction.

        Once all of the bytes have been generated (end of the file) finalizePayload() is called to formulate the resulting binary in a way that
        libc/binloader can understand.

    Implemented, and tested instruction list below. Fields indicate if the path for parsing in solace has been created, if the byte generator is being
    called to formulate the instruction, and the status of a cpputest existing to ensure that the bytegenerator stays conforming to the nabla ASM spec.


    Argument          Parsed            Generated           Tested
    -----------------------------------------------------------------
        add             X                   X                  X
        sub             X                   X                  X 
        mul             X                   X                  X
        div             X                   X                  X
        add.d           X                   X                  X
        sub.d           X                   X                  X
        mul.d           X                   X                  X
        div.d           X                   X                  X
        bgt             X                   X                  X
        bgte            X                   X                  X
        blt             X                   X                  X
        blte            X                   X                  X
        beq             X                   X                  X
        bne             X                   X                  X
        bgt.d           X                   X                  X
        bgte.d          X                   X                  X
        blt.d           X                   X                  X
        blte.d          X                   X                  X
        beq.d           X                   X                  X
        bne.d           X                   X                  X
        mov             X                   X                  X
        ldb             X                   X                  X
        stb             X                   X                  X
        push            X                   X                  X
        pop             X                   X                  X
        jmp             X                   X                  X
        call            X                   X                  X
        ret             X                   X                  X
        exit            X                   X                  X
        lsh             X                   X                  X
        rsh             X                   X                  X
        and             X                   X                  X
        or              X                   X                  X
        xor             X                   X                  X
        not             X                   X                  X
        nop
        label           X                   NA                 NA

*/

#include "solace.hpp"
#include "bytegen.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <regex>
#include <functional>
#include <map>
#include <unordered_map>

namespace SOLACE
{
bool instruction_nop();

bool instruction_lsh();
bool instruction_rsh();
bool instruction_and();
bool instruction_or ();
bool instruction_xor();
bool instruction_not();

bool instruction_add();
bool instruction_sub();
bool instruction_div();
bool instruction_mul();
bool instruction_addd();
bool instruction_subd();
bool instruction_divd();
bool instruction_muld();

bool instruction_mov();
bool instruction_stb();
bool instruction_ldb();
bool instruction_push();
bool instruction_pop();

bool instruction_bgt();
bool instruction_blt();
bool instruction_bgte();
bool instruction_blte();
bool instruction_beq();
bool instruction_bne();

bool instruction_bgtd();
bool instruction_bltd();
bool instruction_bgted();
bool instruction_blted();
bool instruction_beqd();
bool instruction_bned();

bool instruction_jmp();
bool instruction_call();
bool instruction_return();

bool instruction_exit();

bool instruction_directive();
bool instruction_create_function();
bool instruction_end_function();

namespace 
{
    constexpr int      MAXIMUM_STRING_ALLOWED   = 255;
    constexpr uint64_t MAXIMUM_STACK_OFFSET     = 4294967295;
    constexpr uint16_t INPLACE_NUM_RANGE        = 32767;

    NABLA::Bytegen nablaByteGen;

    struct constantdata
    {
        std::string name;
        std::vector<uint8_t> data;
    };

    struct Payload
    {
        //std::unordered_map<std::string, std::vector<uint8_t> > constants;      // Constants

        std::vector<constantdata> constants;
        std::vector<uint8_t> bytes;             // Resulting byte data
        std::string entryPoint;                 // Application entry point

    };

    // The final payload
    Payload finalPayload;

    // Default name given to entry point (defined by .init)
    constexpr char UNDEFINED_ENTRY_POINT[] = "___UNDEFINED__ENTRY__POINT___";
    bool initPointDefined;

    // The current line we are parsing
    std::string currentLine;

    // The pieces of the line we are parsing
    std::vector<std::string> currentPieces;

    // Tie regex matches to function calls that
    struct MatchCall
    {
        std::regex reg;
        std::function<bool()> call;
    };

    std::vector<MatchCall> parserMethods = {
        
        // Directives
        MatchCall{ std::regex("^\\.[a-z0-9]+(8|16|32|64)?$") , instruction_directive },

        MatchCall{ std::regex("^nop$")       , instruction_nop       },

        // Bitwise
        MatchCall{ std::regex("^lsh$")       , instruction_lsh       },
        MatchCall{ std::regex("^rsh$")       , instruction_rsh       },
        MatchCall{ std::regex("^and$")       , instruction_and       },
        MatchCall{ std::regex("^or$" )       , instruction_or        },
        MatchCall{ std::regex("^xor$")       , instruction_xor       },
        MatchCall{ std::regex("^not$")       , instruction_not       },

        // Arithmatic
        MatchCall{ std::regex("^add$")       , instruction_add       },
        MatchCall{ std::regex("^sub$")       , instruction_sub       },
        MatchCall{ std::regex("^mul$")       , instruction_mul       },
        MatchCall{ std::regex("^div$")       , instruction_div       },
        MatchCall{ std::regex("^add\\.d$")   , instruction_addd      },
        MatchCall{ std::regex("^sub\\.d$")   , instruction_subd      },
        MatchCall{ std::regex("^mul\\.d$")   , instruction_muld      },
        MatchCall{ std::regex("^div\\.d$")   , instruction_divd      },

        // Branches
        MatchCall{ std::regex("^bgt$")       , instruction_bgt       },
        MatchCall{ std::regex("^blt$")       , instruction_blt       },
        MatchCall{ std::regex("^bgte$")      , instruction_bgte      },
        MatchCall{ std::regex("^blte$")      , instruction_blte      },
        MatchCall{ std::regex("^beq$")       , instruction_beq       },
        MatchCall{ std::regex("^bne$")       , instruction_bne       },
        MatchCall{ std::regex("^bgt\\.d$")   , instruction_bgtd      },
        MatchCall{ std::regex("^blt\\.d$")   , instruction_bltd      },
        MatchCall{ std::regex("^bgte\\.d$")  , instruction_bgted     },
        MatchCall{ std::regex("^blte\\.d$")  , instruction_blted     },
        MatchCall{ std::regex("^beq\\.d$")   , instruction_beqd      },
        MatchCall{ std::regex("^bne\\.d$")   , instruction_bned      },

        // Data transit
        MatchCall{ std::regex("^mov$")       , instruction_mov       },
        MatchCall{ std::regex("^ldb$")       , instruction_ldb       },
        MatchCall{ std::regex("^stb$")       , instruction_stb       },
        MatchCall{ std::regex("^push$")      , instruction_push      },
        MatchCall{ std::regex("^pop$")       , instruction_pop      },

        // Function movement
        MatchCall{ std::regex("^jmp$")       , instruction_jmp       },
        MatchCall{ std::regex("^call$")      , instruction_call      },
        MatchCall{ std::regex("^ret$")       , instruction_return    },

        MatchCall{ std::regex("^exit$")      , instruction_exit      },

        // Function creation
        MatchCall{ std::regex("^\\<[a-zA-Z0-9_]+:$") , instruction_create_function },
        MatchCall{ std::regex("^\\>$")       , instruction_end_function }
    };

    //         Function             Label       value
    std::map<std::string, std::map<std::string, uint32_t> > preProcessedLabels;

    //        Function    Address
    std::map<std::string, uint32_t> preProcessedFunctions;

    // A temporary function instruction accumulator
    struct FunctionInformation
    {
        std::string name;
        std::vector<uint8_t> instructions;
    };

    FunctionInformation currentFunction;

    bool isSystemBuildingFunction;

    bool isParserVerbose;

    std::vector<std::string> rawFile;

}

// -----------------------------------------------
//
// -----------------------------------------------

inline static std::vector<std::string> chunkLine(std::string line) 
{
    std::vector<std::string> chunks;

    bool inSubStr = false;
    std::string token = "";

    // Why do they even ever use this ? ugh
    if (!line.empty() && line[line.size() - 1] == '\r'){
        line.erase(line.size() - 1);
    }

    for(unsigned i = 0; i < line.length(); i++) {

        if (line[i] == '"') {
            inSubStr ? inSubStr = false : inSubStr = true;
        }

        if ((line[i] == ' ' || line[i] == '\t' || line[i] == '\n') && !inSubStr){
            if (token.length() > 0) {
                chunks.push_back(token);
                token.clear();
            } 
        } else {
            token = token + line[i];
        }
    }

    if (token.length() > 0) {
        chunks.push_back(token);
    }

    return chunks;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static std::string rtrim(std::string &line) 
{
    line.erase(std::find_if(line.rbegin(), line.rend(), 
    [](int ch) 
    {
        return !std::isspace(ch);
    }).base(), line.end());
    return line;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static std::string ltrim (std::string &line)
{
    line.erase(line.begin(),find_if_not(line.begin(),line.end(),
    [](int c)
    {
        return isspace(c);
    }));
    return line;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isFunctionInPayload(std::string name)
{
    return (preProcessedFunctions.find(name) != preProcessedFunctions.end());
}

// -----------------------------------------------
//
// -----------------------------------------------

bool finalizePayload(std::vector<uint8_t> & finalBytes)
{
    if(isParserVerbose) 
    { 
        std::cout << std::endl 
                  << "-----------------------------------" 
                  << std::endl << "Finalizing payload" 
                  << std::endl
                  << "\tFunctions created .... " << preProcessedFunctions.size() << std::endl
                  << "\tTotal bytes .......... " << finalPayload.bytes.size() << std::endl
                  << "-----------------------------------" 
                  << std::endl;
    }

    // Final checks
    if(!isFunctionInPayload(finalPayload.entryPoint))
    {
        std::cerr << "Entry point not defined" << std::endl;
        return false;
    }

    /*
        Create the id that indicates the constant segment is starting
    */
    std::vector<uint8_t> constantStartIns = nablaByteGen.createSegConstInstruction(
        (uint64_t) finalPayload.constants.size()
        );

    for(auto &b : constantStartIns)
    {
        finalBytes.push_back(b);
    }

    /*
        Load all of the constants
    */
    if(isParserVerbose) { std::cout << "Loading " << finalPayload.constants.size() << " constants" << std::endl; }

    for(auto &c : finalPayload.constants)
    {
        if(isParserVerbose) 
        { 
            std::cout << "\tConstant: " << c.name << " [" << c.data.size() << "] bytes..." << std::endl;
        }

        for(auto &b : c.data)
        {
            finalBytes.push_back(b);
        }
    }

    if(isParserVerbose) { std::cout << "Complete" << std::endl; }

    /*
        Create the id that indicates the function segment is starting
    */
    uint64_t entryPointAddress = preProcessedFunctions[finalPayload.entryPoint];

    std::vector<uint8_t> funcSegment = nablaByteGen.createSegFuncInstruction(
        entryPointAddress
    );

    for(auto &b : funcSegment)
    {
        finalBytes.push_back(b);
    }

    /*
        Load all of the bytes
    */
    if(isParserVerbose) { std::cout << "Loading instruction set... "; }
    for(auto& b : finalPayload.bytes)
    {
        finalBytes.push_back(b);
    }
    std::cout << "complete" << std::endl;

    /*
        Create the id that indicates the end of function
    */
    std::vector<uint8_t> binEnd = nablaByteGen.createSegBinEOF();

    for(auto &b : binEnd)
    {
        finalBytes.push_back(b);
    }

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool parseFile(std::string file)
{
    std::ifstream ifs(file);

    if(!ifs.is_open())
    {
        std::cout << "Unable to open file : " <<file << std::endl;
        return false;
    }

    /*
        Load and pre-process
    */

    bool inFunc = false;                        // Ensure we are inside a function before we determine a line was an instruction
    uint64_t preprocessInsCount = 0;            // Count the instructions in the current function to determine label addresses
    uint32_t preprocessFunctionCounter    = 0;  // Count the number of functions so we can determine function address
    std::string cFuncName;                      // The function that we are currently processing

    while (std::getline(ifs, currentLine))
    {
        // Ensure we ignore comments
        currentLine = currentLine.substr(0, currentLine.find(";", 0));

        // Remove potential cruft
        currentLine = ltrim(currentLine);

        if(currentLine.length() > 0)
        {
            // Seperate the lines by 'chunks' (spaces not inside string definitions)
            currentPieces = chunkLine(currentLine);

            if(currentPieces.size() > 0)
            {
                // Always assume that we are accepting the current line as an instruction unless
                // explicitly told not to
                bool acceptLine = true;

                // In a new function
                if(std::regex_match(currentPieces[0], std::regex("^\\<[a-zA-Z0-9_]+:$")))
                {
                    // Indicate that we are in a function
                    inFunc = true;

                    // Get the current function's name
                    cFuncName = currentPieces[0].substr(1, currentPieces[0].size()-2);

                    // Ensure that the function is unique
                    if(preProcessedFunctions.find(cFuncName) != preProcessedFunctions.end())
                    {
                        std::cerr << "Duplicate function '" << cFuncName << "' found" << std::endl;
                        return false;
                    }

                    // Set the address of the function so we can map calls to it in the second pass
                    preProcessedFunctions[cFuncName] = preprocessFunctionCounter;
                }
                // Left a function
                else if (std::regex_match(currentPieces[0], std::regex("^\\>$")))
                {
                    // Indicate we aren't in a function anymore
                    inFunc = false;

                    // Reset the instruction count, because we count per-function for labels
                    preprocessInsCount = 0;

                    // Increaase the function counter so the next function can get the appropriate address
                    preprocessFunctionCounter++;
                }
                // Found a label
                else if(inFunc && std::regex_match(currentPieces[0], std::regex("^[a-zA-Z0-9_]+:$")))
                {
                    // get the label string value
                    std::string label = currentPieces[0] .substr(0, currentPieces[0].size()-1);

                    // Ensure that the label isnt a duplicate
                    if(preProcessedLabels[cFuncName].find(label) !=  preProcessedLabels[cFuncName].end())
                    {
                        std::cerr << "Duplicate label '" << label << "' found in function '" << cFuncName << "'" << std::endl;
                        return false;
                    }

                    // Create a label in the map of maps that maps to the area the jmp or branch instruction should go to
                    preProcessedLabels[cFuncName][label] = preprocessInsCount;

                    // Ensure that we don't count this line as an instruction
                    acceptLine = false;
                }
                else if (inFunc)
                {
                    // Increase the instruction count
                    preprocessInsCount++;
                }

                // If we are to put the line in for actual processing (i.e not a label) add it to the rawFile
                // for the second pass
                if(acceptLine)
                {
                    rawFile.push_back(currentLine);
                }
            }
        }
    }

    ifs.close();

    for(uint64_t cline = 0; cline < rawFile.size(); cline++)
    {
        currentLine = rawFile[cline];

        std::cout << currentLine << std::endl;

        currentPieces = chunkLine(currentLine);

        bool found = false;
        for(auto &i : parserMethods)
        {
            // Match what the line means to do, and call the function to handle it
            if(std::regex_match(currentPieces[0], i.reg))
            {
                // If the call returns false, an error popped up
                if(!i.call())
                {
                    return false;
                }

                found = true;
            }
        }

        if(!found)
        {
            std::cerr << "Unknown instruction(s) : " << currentLine << std::endl;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool ParseAsm(std::string asmFile, std::vector<uint8_t> &bytes, bool verbose)
{
    // Set verbosity
    isParserVerbose = verbose;

    // Mark the entry point as undefined to start
    finalPayload.entryPoint = UNDEFINED_ENTRY_POINT;

    // Indicate that we haven't seen a definition for the entry point yet
    initPointDefined = false;

    currentFunction.name = "UNDEFINED";
    isSystemBuildingFunction = false;

    // Parse the first file - Include directives will drive the parsing
    // of required files
    if(!parseFile(asmFile))
    {
        return false;
    }

    // Finalize the payload, and set the bytes so the caller has something they
    // can play with
    return finalizePayload(bytes);
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isBranchableLabel(std::string name)
{
    return std::regex_match(name, std::regex("^[a-zA-Z_0-9]+$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isConstNameValid(std::string name)
{
    return std::regex_match(name, std::regex("^[a-zA-Z_0-9]+"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isInteger(std::string name)
{
    return std::regex_match(name, std::regex("(^[0-9]+$|^\\-[0-9]+$)"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isDouble(std::string piece)
{
    return std::regex_match(piece, std::regex("^[0-9]+.[0-9]+$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isRegister(std::string piece)
{
    return std::regex_match(piece, std::regex("^r{1}([0-9]|1[0-5])$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isSystemRegister(std::string piece)
{
    return std::regex_match(piece, std::regex("^sys{1}[0-1]$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isDirectLocalStackPointer(std::string piece)
{
    return std::regex_match(piece, std::regex("^ls$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isDirectGlobalStackPointer(std::string piece)
{
    return std::regex_match(piece, std::regex("^gs$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

static inline uint32_t getOffsetFromStackOffset(std::string piece)
{
    std::string str = piece.substr(1, piece.size()-5);

    uint32_t n = std::stoi(str);

    return n;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isStackOffsetInRange(std::string piece)
{
    return ( getOffsetFromStackOffset(piece) < MAXIMUM_STACK_OFFSET );
}


// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isOffsetLocalStackpointer(std::string piece)
{
    if( std::regex_match(piece, std::regex("^\\$[0-9]+\\(ls\\)$")) )
    {
        return isStackOffsetInRange(piece);
    }
    return false;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isOffsetGlobalStackpointer(std::string piece)
{
    if( std::regex_match(piece, std::regex("^\\$[0-9]+\\(gs\\)$")) )
    {
        return isStackOffsetInRange(piece);
    }
    return false;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static int getNumberFromNumericalOrRegister(std::string numerical)
{
    std::string str = numerical.substr(1, numerical.size());

    return std::stoi(str);
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isDirectNumericalInRange(std::string numerical)
{
    int n = getNumberFromNumericalOrRegister(numerical);

    return ( n < INPLACE_NUM_RANGE && n > - INPLACE_NUM_RANGE );
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isDirectNumerical(std::string piece)
{
    if(std::regex_match(piece, std::regex("(^\\$[0-9]+$)|(^\\$\\-[0-9]+$)")))
    {
        return isDirectNumericalInRange(piece);
    }
    return false;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isDirectNumericalDouble(std::string piece)
{
    return std::regex_match(piece, std::regex("^\\$[0-9]+.[0-9]+$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isReferencedConstant(std::string piece)
{
    return std::regex_match(piece, std::regex("^\\&[a-zA-Z_0-9]+$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isString(std::string piece)
{
    return std::regex_match(piece, std::regex("^\".*\"$"));
}

// -----------------------------------------------
// Check if something is stored as a constant or not
// -----------------------------------------------

inline static bool isConstInPayload(std::string name)
{
    for(auto &c : finalPayload.constants)
    {
        if(name == c.name)
        {
            return true;
        }
    }
    return false;
}

// -----------------------------------------------
//  getConstIndex doesn't care if something exists or not. Do that yourself.
// -----------------------------------------------

inline static uint32_t getConstIndex(std::string name)
{
    uint32_t i = 0;
    for(auto &c : finalPayload.constants)
    {
        if(name == c.name)
        {
            return i;
        }
        i++;
    }
    return i;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isLabelInCurrentFunction(std::string name)
{
    return (preProcessedLabels[currentFunction.name].find(name) !=  preProcessedLabels[currentFunction.name].end());
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static void addBytegenInstructionToPayload(NABLA::Bytegen::Instruction ins)
{
    finalPayload.bytes.push_back(ins.bytes[0]); 
    finalPayload.bytes.push_back(ins.bytes[1]); 
    finalPayload.bytes.push_back(ins.bytes[2]);
    finalPayload.bytes.push_back(ins.bytes[3]); 
    finalPayload.bytes.push_back(ins.bytes[4]); 
    finalPayload.bytes.push_back(ins.bytes[5]);
    finalPayload.bytes.push_back(ins.bytes[6]); 
    finalPayload.bytes.push_back(ins.bytes[7]);
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static void addBytegenInstructionToCurrentFunction(NABLA::Bytegen::Instruction ins)
{
    currentFunction.instructions.push_back(ins.bytes[0]); 
    currentFunction.instructions.push_back(ins.bytes[1]);
    currentFunction.instructions.push_back(ins.bytes[2]); 
    currentFunction.instructions.push_back(ins.bytes[3]);
    currentFunction.instructions.push_back(ins.bytes[4]); 
    currentFunction.instructions.push_back(ins.bytes[5]);
    currentFunction.instructions.push_back(ins.bytes[6]); 
    currentFunction.instructions.push_back(ins.bytes[7]);
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static std::string convertArithToString(NABLA::Bytegen::ArithmaticTypes type)
{
    switch(type)
    {
        case NABLA::Bytegen::ArithmaticTypes::ADD : return "ADD";
        case NABLA::Bytegen::ArithmaticTypes::ADDD: return "ADDD";
        case NABLA::Bytegen::ArithmaticTypes::MUL : return "MUL";
        case NABLA::Bytegen::ArithmaticTypes::MULD: return "MULD";
        case NABLA::Bytegen::ArithmaticTypes::DIV : return "DIV";
        case NABLA::Bytegen::ArithmaticTypes::DIVD: return "DIVD";
        case NABLA::Bytegen::ArithmaticTypes::SUB : return "SUB";
        case NABLA::Bytegen::ArithmaticTypes::SUBD: return "SUBD";
        default:                    return "UNKNOWN"; // Keep that compiler happy.
    }
}

// -----------------------------------------------
// 
// -----------------------------------------------

inline static bool arithmatic_instruction(NABLA::Bytegen::ArithmaticTypes type)
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }

    if(isParserVerbose) { std::cout << "Arithmatic Instruction : " << currentLine << std::endl; }

    bool isArg2Register = true;
    bool isArg3Register = true;

    // These are the non-double commands
    if(static_cast<unsigned>(type) <= 0x03)
    {
        // --------------------------------------------
        //  ARG 1
        // --------------------------------------------

        // Check if is register
        if(!isRegister(currentPieces[1]))
        {
            std::cerr << convertArithToString(type) << "::arg1::not_matched, given :" << currentPieces[1] << std::endl;
            return false;
        }

        // --------------------------------------------
        //  ARG 2
        // --------------------------------------------

        if(isRegister(currentPieces[2]))
        {
            //std::cout << "Register : " << currentPieces[2] << std::endl;
        }
        else if(isDirectNumerical(currentPieces[2]))
        {
            if(isParserVerbose){ std::cout << "Direct Numerical : " << currentPieces[2] << std::endl; }

            if(!isDirectNumericalInRange(currentPieces[2]))
            {
                std::cerr << "Error: Direct numerical insert is out of range: " << currentPieces[2] << std::endl;
                return false;
            }

            isArg2Register = false;
        }
        else
        {
            std::cerr << convertArithToString(type) << "::arg2::not_matched, given :" << currentPieces[2] << std::endl;
            return false;
        }

        // --------------------------------------------
        //  ARG 3
        // --------------------------------------------

        if(isRegister(currentPieces[3]))
        {
            if(isParserVerbose){ std::cout << "Register : " << currentPieces[3] << std::endl; }
        }
        else if(isDirectNumerical(currentPieces[3]))
        {
            if(isParserVerbose){ std::cout << "Direct Numerical : " << currentPieces[3] << std::endl; }

            if(!isDirectNumericalInRange(currentPieces[3]))
            {
                std::cerr << "Error: Direct numerical insert is out of range: " << currentPieces[3] << std::endl;
                return false;
            }

            isArg3Register = false;
        }
        else
        {
            std::cerr << convertArithToString(type) << "::arg3::not_matched, given :" << currentPieces[3] << std::endl;
            return false;
        }
    } 
    //  ---------------------------------------------------
    //      Check the double types!
    //  ---------------------------------------------------
    else 
    {
        // --------------------------------------------
        //  ARG 1
        // --------------------------------------------

        // Check if is register
        if(isRegister(currentPieces[1]))
        {
            if(isParserVerbose){ std::cout << convertArithToString(type) << "::arg1::register::" << currentPieces[1] << std::endl; }
        }
        else
        {
            std::cerr << convertArithToString(type) << "::arg1::not_matched, given :" << currentPieces[1] << std::endl;
            return false;
        }

        // --------------------------------------------
        //  ARG 2
        // --------------------------------------------

        if(isRegister(currentPieces[2]))
        {
            if(isParserVerbose){ std::cout << "Double Register : " << currentPieces[2] << std::endl; }
        }
        else
        {
            std::cerr << convertArithToString(type) << "::arg2::not_matched, given :" << currentPieces[2] << std::endl;
            return false;
        }

        // --------------------------------------------
        //  ARG 3
        // --------------------------------------------

        if(isRegister(currentPieces[3]))
        {
            if(isParserVerbose){ std::cout << "Register : " << currentPieces[3] << std::endl; }
        }
        else
        {
            std::cerr << convertArithToString(type) << "::arg3::not_matched, given :" << currentPieces[3] << std::endl;
            return false;
        }
    }

    // Get numerical values of the numbers OR regs
    int16_t arg1 = getNumberFromNumericalOrRegister(currentPieces[1]);
    int16_t arg2 = getNumberFromNumericalOrRegister(currentPieces[2]);
    int16_t arg3 = getNumberFromNumericalOrRegister(currentPieces[3]);

    // Determine how we want to inform the bytegen whats going on 
    NABLA::Bytegen::ArithmaticSetup setup;
    if (isArg2Register && isArg3Register)       { setup = NABLA::Bytegen::ArithmaticSetup::REG_REG; }
    else if (isArg2Register && !isArg3Register) { setup = NABLA::Bytegen::ArithmaticSetup::REG_NUM; }
    else if (isArg3Register && !isArg2Register) { setup = NABLA::Bytegen::ArithmaticSetup::NUM_REG; }
    else                                        { setup = NABLA::Bytegen::ArithmaticSetup::NUM_NUM; }

    // Call to create the instruction and add to current function
    addBytegenInstructionToCurrentFunction(nablaByteGen.createArithmaticInstruction(type, setup, arg1, arg2, arg3));

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_add()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'add' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(NABLA::Bytegen::ArithmaticTypes::ADD);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_sub()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'sub' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(NABLA::Bytegen::ArithmaticTypes::SUB);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_div()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'div' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(NABLA::Bytegen::ArithmaticTypes::DIV);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_mul()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'mul' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(NABLA::Bytegen::ArithmaticTypes::MUL);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_addd()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'addd' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(NABLA::Bytegen::ArithmaticTypes::ADDD);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_subd()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'subd' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(NABLA::Bytegen::ArithmaticTypes::SUBD);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_divd()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'divd' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(NABLA::Bytegen::ArithmaticTypes::DIVD);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_muld()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'muld' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(NABLA::Bytegen::ArithmaticTypes::MULD);
}

// -----------------------------------------------
// Parsed, not complete
// -----------------------------------------------

bool instruction_mov()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }

    if(currentPieces.size() != 3)
    {
        std::cerr << "Invalid 'mov' instruction : " << currentLine << std::endl;
        return false;
    }

    if(!isRegister(currentPieces[1]))
    {
        std::cerr << "Argument 1 of 'mov' must be a register" << std::endl;
        return false;
    }

    NABLA::Bytegen::MovSetup setup;

    if(isRegister(currentPieces[2]))
    {
        setup = NABLA::Bytegen::MovSetup::REG_REG;
    }
    else if (isDirectNumerical(currentPieces[2]))
    {
        setup = NABLA::Bytegen::MovSetup::REG_NUM;
    }
    else
    {
        std::cerr << "Argument 2 of 'mov' must be a register or a direct numerical constant" << std::endl;
        return false;
    }

    if(isParserVerbose) { std::cout << "Creating mov instruction : " << currentLine << std::endl; }

    // Both are confirmed registers!
    uint8_t reg1 = getNumberFromNumericalOrRegister(currentPieces[1]);
    uint8_t reg2 = getNumberFromNumericalOrRegister(currentPieces[2]);

    // Generate the bytes and add to the current function
    addBytegenInstructionToCurrentFunction(
        nablaByteGen.createMovInstruction(setup, reg1, reg2)
        );
    return true;
}

// -----------------------------------------------
// 
// -----------------------------------------------

bool instruction_stb()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }

    std::cout << "stb : " << currentLine << std::endl;

    if(currentPieces.size() != 3)
    {
        std::cerr << "Invalid 'stb' instruction : " << currentLine << std::endl;
        return false;
    }

    NABLA::Bytegen::Stacks stackType;

    // Check argument 1 
    if(isOffsetGlobalStackpointer(currentPieces[1]))
    {
        if(isParserVerbose){ std::cout << "Argument 1 is global spo : " << currentPieces[1] << std::endl; }

        stackType = NABLA::Bytegen::Stacks::GLOBAL;
    }
    else if (isOffsetLocalStackpointer(currentPieces[1]))
    {
        if(isParserVerbose){ std::cout << "Argument 1 is local spo : " << currentPieces[1] << std::endl; }
        
        stackType = NABLA::Bytegen::Stacks::LOCAL;
    }
    else
    {
        std::cerr << "'stb' argument 1 must be a stack pointer offset, got : " << currentPieces[1] << " instread" << std::endl;
        return false;
    }

    if(!isStackOffsetInRange(currentPieces[1]))
    {
        std::cerr << "Stack offset [" << currentPieces[1] << "] is out of the acceptable range of offsets" << std::endl;
        return false;
    }
    
    if(!isRegister(currentPieces[2]))
    {
        std::cerr << "'stb' argument 2 must be a register, got : " << currentPieces[2] << " instead" << std::endl;
        return false;
    }

    uint32_t location = getOffsetFromStackOffset(currentPieces[1]);
    uint8_t reg = getNumberFromNumericalOrRegister(currentPieces[2]);

    addBytegenInstructionToCurrentFunction(
        nablaByteGen.createStbInstruction(stackType, location, reg)
        );

    return true;
}

// -----------------------------------------------
// 
// -----------------------------------------------

bool instruction_ldb()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }

    std::cout << "ldb : " << currentLine << std::endl;

    if(currentPieces.size() != 3)
    {
        std::cerr << "Invalid 'ldb' instruction : " << currentLine << std::endl;
        return false;
    }

    if(!isRegister(currentPieces[1]))
    {
        std::cerr << "Error: First argument of 'ldb' must be a register, got :" << currentPieces[1] << std::endl;
        return false;
    }

    NABLA::Bytegen::Stacks stackType;
    // Check if its a global stack pointer
    if (isOffsetGlobalStackpointer(currentPieces[2]))
    {
        if(isParserVerbose){ std::cout << "Argument 2 -> global_stack_pointer_offset " << std::endl; }
    
        stackType = NABLA::Bytegen::Stacks::GLOBAL;
    }

    // Check if its a local stack pointer
    else if (isOffsetLocalStackpointer(currentPieces[2]))
    {
        if(isParserVerbose){  std::cout << "Argument 2 -> local_stack_pointer_offset " << std::endl; }
        
        stackType = NABLA::Bytegen::Stacks::LOCAL;
    }
    else
    {
        std::cerr << "'ldb' argument 2 must be stack offset, got : " << currentPieces[2] << std::endl;
        return false;
    }

    uint32_t location = getOffsetFromStackOffset(currentPieces[2]);
    uint8_t reg = getNumberFromNumericalOrRegister(currentPieces[1]);

    addBytegenInstructionToCurrentFunction(
        nablaByteGen.createLdbInstruction(stackType, location, reg)
        );

    return true;
}

// -----------------------------------------------
// 
// -----------------------------------------------

bool instruction_push()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }

    if(!currentPieces.size() == 3)
    {
        std::cerr << "Invalid 'push' instruction : " << currentLine << std::endl;
        return false; 
    }

    if(isParserVerbose){ std::cout << "push : " << currentLine << std::endl; }

    NABLA::Bytegen::Stacks stackType;

    // Argument 1
    if(isDirectLocalStackPointer(currentPieces[1]))
    {
        if(isParserVerbose){ std::cout << "Argument 1 is a local stack pointer " << std::endl; }

        stackType = NABLA::Bytegen::Stacks::LOCAL;
    }
    else if (isDirectGlobalStackPointer(currentPieces[1]))
    {
        if(isParserVerbose){ std::cout << "Argument 1 is a global stack pointer " << std::endl; }
        
        stackType = NABLA::Bytegen::Stacks::GLOBAL;
    }
    else
    {
        std::cerr << "'push' instruction argument 1 must be a global or local stack pointer" << std::endl;
        return false;
    }
    
    // Argument 2
    if(!isRegister(currentPieces[2]))
    {
        std::cout << "'push' instruction argument 2 must be a register" << std::endl;
        return false;
    }

    uint8_t reg = getNumberFromNumericalOrRegister(currentPieces[2]);
    
    addBytegenInstructionToCurrentFunction(
        nablaByteGen.createPushInstruction(stackType, reg)
        );

    return true;
}

// -----------------------------------------------
// 
// -----------------------------------------------

bool instruction_pop()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }

    if(!currentPieces.size() == 3)
    {
        std::cerr << "Invalid 'pop' instruction : " << currentLine << std::endl;
        return false; 
    }

    if(isParserVerbose){ std::cout << "pop : " << currentLine << std::endl; }

    // Argument 1
    if(!isRegister(currentPieces[1]))
    {
        std::cerr << "'pop' instruction argument 1 must be a register" << std::endl;
        return false;
    }

    NABLA::Bytegen::Stacks stackType;

    // Argument 2
    if(isDirectLocalStackPointer(currentPieces[2]))
    {
        if(isParserVerbose){ std::cout << "Argument 2 is a local stack pointer " << std::endl; }

        stackType = NABLA::Bytegen::Stacks::LOCAL;
    }
    else if (isDirectGlobalStackPointer(currentPieces[2]))
    {
        if(isParserVerbose){ std::cout << "Argument 2 is a global stack pointer " << std::endl; }
        
        stackType = NABLA::Bytegen::Stacks::GLOBAL;
    }
    else
    {
        std::cerr << "'pop' instruction argument 2 must be a global or local stack pointer (not an offset)" << std::endl;
        return false;
    }
    
    uint8_t reg = getNumberFromNumericalOrRegister(currentPieces[1]);
    
    addBytegenInstructionToCurrentFunction(
        nablaByteGen.createPopInstruction(stackType, reg)
        );

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static std::string convertBranchToString(NABLA::Bytegen::BranchTypes type)
{
    switch(type)
    {
        case NABLA::Bytegen::BranchTypes::BGT  : return "BGT  "; 
        case NABLA::Bytegen::BranchTypes::BGTE : return "BGTE "; 
        case NABLA::Bytegen::BranchTypes::BLT  : return "BLT  "; 
        case NABLA::Bytegen::BranchTypes::BLTE : return "BLTE "; 
        case NABLA::Bytegen::BranchTypes::BEQ  : return "BEQ  "; 
        case NABLA::Bytegen::BranchTypes::BNE  : return "BNE  "; 
        case NABLA::Bytegen::BranchTypes::BGTD : return "BGTD ";
        case NABLA::Bytegen::BranchTypes::BGTED: return "BGTED";
        case NABLA::Bytegen::BranchTypes::BLTD : return "BLTD ";
        case NABLA::Bytegen::BranchTypes::BLTED: return "BLTED";
        case NABLA::Bytegen::BranchTypes::BEQD : return "BEQD ";
        case NABLA::Bytegen::BranchTypes::BNED : return "BNED ";
        default:                 return "UNKNOWN"; // Keep that compiler happy.
    }
}

inline bool branch_instruction(NABLA::Bytegen::BranchTypes type)
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }

    if(!isRegister(currentPieces[1]))
    {
        std::cerr << "branch argument 1 must be a register" << std::endl;
        return false;
    }

    if(!isRegister(currentPieces[2]))
    {
        std::cerr << "branch argument 2 must be a register" << std::endl;
        return false;
    }

    if(!isBranchableLabel(currentPieces[3]))
    {
        std::cerr << "branch argument 3 must be a label, got : " << currentPieces[3] <<  std::endl;
        return false;
    }

    if(!isLabelInCurrentFunction(currentPieces[3]))
    {
        std::cerr << "branches must branch to existing labels. - We hope to extend this functionality later" << std::endl;
        return false;
    }

    uint32_t labelValue = preProcessedLabels[currentFunction.name][currentPieces[3]];

    if(isParserVerbose) { std::cout << currentPieces[0] << " Arg1: " 
                          << currentPieces[1]           << " Arg2: "
                          << currentPieces[2]           << " Label: "
                          << currentPieces[3]           << " Label Address: "
                          << labelValue <<  std::endl; }

    // We are expecting registers as regex magic has already deemed them fit to be registers.
    // The following method simply peels off the 'r' so we are left with the integer 
    uint8_t reg1 = getNumberFromNumericalOrRegister(currentPieces[1]);
    uint8_t reg2 = getNumberFromNumericalOrRegister(currentPieces[2]);

    // Generate the bytes and add to the current function
    addBytegenInstructionToCurrentFunction(
        nablaByteGen.createBranchInstruction(type, reg1, reg2, labelValue)
        );

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_bgt()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'bgt' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BGT);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_blt()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'blt' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BLT);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_bgte()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'bgte' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BGTE);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_blte()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'blte' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BLTE);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_beq()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'beq' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BEQ);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_bne()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'bne' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BNE);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_bgtd()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'bgtd' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BGTD);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_bltd()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'bltd' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BLTD);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_bgted()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'bgted' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BGTED);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_blted()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'blted' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BLTED);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_beqd()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'beqd' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BEQD);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_bned()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'bned' instruction : " << currentLine << std::endl;
        return false;
    }

    return branch_instruction(NABLA::Bytegen::BranchTypes::BNED);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_jmp()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }

    if(!currentPieces.size() == 2)
    {
        std::cerr << "Invalid jump instruction : " << currentLine << std::endl;
        return false;
    }
    
    // Same as branch.. sue me.
    if(!isBranchableLabel(currentPieces[1]))
    {
        std::cerr << "Jump argument 1 must be a label, got : " << currentPieces[1] <<  std::endl;
        return false;
    }

    if(!isLabelInCurrentFunction(currentPieces[1]))
    {
        std::cerr << "Jumps must jump to existing labels. - We hope to extend this functionality later" << std::endl;
        return false;
    }

    if(isParserVerbose) { std::cout << "Creating jmp instruction to : " << currentPieces[1] << std::endl; }

    uint32_t labelValue = preProcessedLabels[currentFunction.name][currentPieces[1]];

    addBytegenInstructionToCurrentFunction(
        nablaByteGen.createJumpInstruction(labelValue)
        );
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_call()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }

    if(currentPieces.size() != 2)
    {
        std::cerr << "Invalid call instruction : " << currentLine << std::endl;
        return false;
    }

    if(!isFunctionInPayload(currentPieces[1]))
    {
        std::cerr << "Function [" << currentPieces[1] << "] has not been declared" << std::endl;
        return false;
    }

    uint32_t currentAddress = nablaByteGen.getCurrentFunctionCouner();
    uint32_t returnArea     = (currentFunction.instructions.size()/8) +3; // Have to add extra b/c hidden generated instructions
    uint32_t destination    = preProcessedFunctions[currentPieces[1]];


    std::cout << "Creating call from : " <<
     currentFunction.name << " @ " << currentAddress << " ret area : "
      << returnArea << " dest : " << currentPieces[1] << " @ " << preProcessedFunctions[currentPieces[1]] << std::endl;


    std::vector<NABLA::Bytegen::Instruction> ins = nablaByteGen.createCallInstruction(
        currentAddress,
        returnArea,
        destination
    );

    for(auto &i : ins)
    {
        addBytegenInstructionToCurrentFunction(i);
    }
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_return()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }
    
    if(currentPieces.size() != 1)
    {
        std::cerr << "Invalid return instruction : " << currentLine << std::endl;
        return false;
    }

    addBytegenInstructionToCurrentFunction(
        nablaByteGen.createReturnInstruction()
        );

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_exit()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "All Instructions must exist within a function" << std::endl;
        return false;
    }
    
    if(currentPieces.size() != 1)
    {
        std::cerr << "Invalid exit instruction : " << currentLine << std::endl;
        return false;
    }

    addBytegenInstructionToCurrentFunction(
        nablaByteGen.createExitInstruction()
        );

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_directive()
{
    if(isSystemBuildingFunction)
    {
        std::cerr << "All directives must happen outside of functions. Currently in function : " << currentFunction.name << std::endl;
        return false;
    }

    if(currentPieces.size() < 2)
    {
        std::cerr << "Unknown potential directive : " << currentLine << std::endl;
        return false;
    }

    /*
        DEVELOPMENT_NOTE
    
            Consider adding the following :

                .float 

                .prototype

            Right now constant strings are limited to 255 chars long. Which is like.. fine.. but
            we could totally do something like .long_string and set a larger size indication field.

            Of course, any of these additions would need to be written-out in the instruction manual first    
    */

    // ----------------------------------------------------------------------
    //  Set entry point
    // ----------------------------------------------------------------------
    if(std::regex_match(currentPieces[0], std::regex("^\\.init$")))
    {
        if(isParserVerbose){ std::cout << "\tinit: " << currentLine << std::endl; }

        if(!std::regex_match(currentPieces[1], std::regex("^[a-zA-Z0-9_]+$")))
        {
            std::cerr << "Invalid name given for .init  : " << currentPieces[1] << std::endl;
            return false;
        }

        if(finalPayload.entryPoint != UNDEFINED_ENTRY_POINT)
        {
            std::cerr << "Entry point already defined as [" 
                      << finalPayload.entryPoint 
                      << "] but [" 
                      << currentPieces[1] 
                      << "] was requested" << std::endl;
            return false;
        }

        finalPayload.entryPoint = currentPieces[1];
    }
    // ----------------------------------------------------------------------
    //  Set .file
    // ----------------------------------------------------------------------
    else if (std::regex_match(currentPieces[0], std::regex("^\\.file$")))
    {
        if(isParserVerbose){ std::cout << "\tfile: " << currentPieces[1] << std::endl; }
    }
    // ----------------------------------------------------------------------
    //  Create a .string constant
    // ----------------------------------------------------------------------
    else if (std::regex_match(currentPieces[0], std::regex("^\\.string$"))) 
    {
        if(isParserVerbose){ std::cout << "\tstring: " << currentLine << std::endl; }

        // Make sure everything is there
        if(currentPieces.size() < 3)
        {
            std::cerr << "Invalid .string constant detected. Requires : .string CONST_NAME \"str\", but [" << currentLine << "] given." << std::endl;
            return false;
        }

        // Ensure name is valid
        if(!isConstNameValid(currentPieces[1]))
        {
            std::cerr << "Invalid name given to constant .string : " << currentPieces[1] << std::endl;
            return false;
        }

        // Ensure the thing is a string
        if(!isString(currentPieces[2]))
        {
            std::cerr << "Invalid type given to constant .string : " << currentPieces[2] << std::endl;
            return false;
        }

        // Clean up the string
        std::string str = currentPieces[2];

        str = ltrim(str);
        str = rtrim(str);
        str = str.substr(1, str.size()-2);

        if(str.size() == 0)
        {
            std::cerr << "Constant .string " << currentPieces[1] << " was determined to be empty after removeing \"s" << std::endl;
            return false;
        }

        // Ensure it isn't too big
        if(str.size() > MAXIMUM_STRING_ALLOWED)
        {
            std::cerr << "Constant string exceeds allowed maximum : " << (int)MAXIMUM_STRING_ALLOWED << std::endl;
            return false;
        }

        // Ensure that we haven't had it defined yet
        if(isConstInPayload(currentPieces[1]))
        {
            std::cerr << "Constant .string " << currentPieces[1] << " previously defined with value."<< std::endl;
            return false;
        }

        // Store it
        finalPayload.constants.push_back({currentPieces[1], nablaByteGen.createConstantString(currentPieces[2])});


    }

    // ----------------------------------------------------------------------
    //  Create a .int constant
    // ----------------------------------------------------------------------
    else if (std::regex_match(currentPieces[0], std::regex("^\\.int8$"))  || std::regex_match(currentPieces[0], std::regex("^\\.int16$")) ||
             std::regex_match(currentPieces[0], std::regex("^\\.int32$")) || std::regex_match(currentPieces[0], std::regex("^\\.int64$"))) 
    {
        if(isParserVerbose){ std::cout << "\tint: " << currentLine << std::endl; }

        // Make sure everything is there
        if(currentPieces.size() < 3)
        {
            std::cerr << "Invalid .int constant detected. Requires : .int CONST_NAME N, but [" << currentLine << "] given." << std::endl;
            return false;
        }

        // Ensure name is valid
        if(!isConstNameValid(currentPieces[1]))
        {
            std::cerr << "Invalid name given to constant .int : " << currentPieces[1] << std::endl;
            return false;
        }

        // Ensure the thing is an integer
        if(!isInteger(currentPieces[2]))
        {
            std::cerr << "Invalid type given to constant .int : " << currentPieces[2] << std::endl;
            return false;
        }

        // Ensure that we haven't had it defined yet
        if(isConstInPayload(currentPieces[1]))
        {
            std::cerr << "Constant .int " << currentPieces[1] << " previously defined" << std::endl;
            return false;
        }

        // Get the int
        uint64_t givenInt = std::stoll(currentPieces[2]);

        NABLA::Bytegen::Integers integerType;
        if(currentPieces[0] == ".int8")     { integerType = NABLA::Bytegen::Integers::EIGHT; }
        else if(currentPieces[0] == ".int16"){ integerType = NABLA::Bytegen::Integers::SIXTEEN; }
        else if(currentPieces[0] == ".int32"){ integerType = NABLA::Bytegen::Integers::THIRTY_TWO; }
        else if(currentPieces[0] == ".int64"){ integerType = NABLA::Bytegen::Integers::SIXTY_FOUR; }
        else {
            std::cerr << "Unknown interger type that was matched : " << currentLine << std::endl;
            return false;
        }

        finalPayload.constants.push_back({currentPieces[1], nablaByteGen.createConstantInt(
            givenInt, 
            integerType      
        )});
    }
    // ----------------------------------------------------------------------
    //  Create a .double constant
    // ----------------------------------------------------------------------
    else if (std::regex_match(currentPieces[0], std::regex("^\\.double$"))) 
    {
        if(isParserVerbose){ std::cout << "\tdouble: " << currentLine << std::endl; }

        // Make sure everything is there
        if(currentPieces.size() < 3)
        {
            std::cerr << "Invalid .double constant detected. Requires : .double CONST_NAME N.M, but [" << currentLine << "] given." << std::endl;
            return false;
        }

        // Ensure name is valid
        if(!isConstNameValid(currentPieces[1]))
        {
            std::cerr << "Invalid name given to constant .double : " << currentPieces[1] << std::endl;
            return false;
        }

        // Ensure the thing is an double
        if(!isDouble(currentPieces[2]))
        {
            std::cerr << "Invalid type given to constant .double : " << currentPieces[2] << std::endl;
            return false;
        }

        // Ensure that we haven't had it defined yet
        if(isConstInPayload(currentPieces[1]))
        {
            std::cerr << "Constant .double " << currentPieces[1] << " previously defined" << std::endl;
            return false;
        }

        // Get the int
        double givenDouble = std::stod(currentPieces[2]);

        // Store it
        finalPayload.constants.push_back({currentPieces[1], nablaByteGen.createConstantDouble(givenDouble)});
    }
    // ----------------------------------------------------------------------
    //  Undefined directive
    // ----------------------------------------------------------------------
    else 
    {
        std::cerr << "Unknown potential directive : " << currentLine << std::endl;
        return false;
    }
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_create_function()
{
    if(isSystemBuildingFunction)
    {
        std::cerr << "Can not create function inside of another function" << std::endl;
        return false;
    }

    if(currentPieces.size() > 1)
    {
        std::cerr << "Function definition invalid : " << currentLine << std::endl;
        return false;
    }

    //std::cout << "Create function: " << currentLine << std::endl;

    std::string functionName = currentPieces[0].substr(1, currentPieces[0].size()-2);
    
    // Ensure we have been told an entry point
    if(finalPayload.entryPoint == UNDEFINED_ENTRY_POINT)
    {
        std::cerr << "Initial entry point (.init) must be declared prior to the existence of any function. Please declare at the top of the main file" << std::endl;
        return false;
    }

    // Check if the unique function is the entry point under the condition that we haven't found it yet
    if(!initPointDefined)
    {
        if (functionName == finalPayload.entryPoint)
        {
            initPointDefined = true;
        }
    }

    // Ask to create the function. 

    isSystemBuildingFunction = true;

    currentFunction.name = functionName;
    currentFunction.instructions.clear();

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_end_function()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "Found stray function end. Not currently building a function" << std::endl;
        return false;
    }
    
    if(currentPieces.size() != 1)
    {
        std::cerr << "Invalid function end. Expected '>' got : " << currentLine << std::endl;
        return false;
    }

    uint32_t functionAddress = 0;

    NABLA::Bytegen::Instruction funcCreate = nablaByteGen.createFunctionStart(currentFunction.name, 
                                                                       (uint64_t)currentFunction.instructions.size(), 
                                                                       functionAddress);
    if(isParserVerbose)
    {
         std::cout << "Created function : " << currentFunction.name << " at address " << (int)functionAddress
                   << " with " << currentFunction.instructions.size() << " bytes " << std::endl;
    }

    addBytegenInstructionToPayload(funcCreate);

    for(auto & ins : currentFunction.instructions )
    {
        finalPayload.bytes.push_back(ins);
    }

    NABLA::Bytegen::Instruction funcEnd = nablaByteGen.createFunctionEnd();

    addBytegenInstructionToPayload(funcEnd);

    isSystemBuildingFunction = false;
    currentFunction.name = "UNDEFINED";
    currentFunction.instructions.clear();

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static std::string convertBitwiseToString(NABLA::Bytegen::BitwiseTypes type)
{
    switch(type)
    {
        case NABLA::Bytegen::BitwiseTypes::LSH : return "LSH"; 
        case NABLA::Bytegen::BitwiseTypes::RSH : return "RSH"; 
        case NABLA::Bytegen::BitwiseTypes::AND : return "AND"; 
        case NABLA::Bytegen::BitwiseTypes::OR  : return "OR "; 
        case NABLA::Bytegen::BitwiseTypes::XOR : return "XOR"; 
        case NABLA::Bytegen::BitwiseTypes::NOT : return "NOT";
        default:                 return "UNKNOWN"; // Keep that compiler happy.
    }
}

// -----------------------------------------------
//
// -----------------------------------------------

bool bitwise_instruction(NABLA::Bytegen::BitwiseTypes type)
{
    
    bool isArg2Register = true;
    bool isArg3Register = true;

    // --------------------------------------------
    //  ARG 1
    // --------------------------------------------

    // Check if is register
    if(!isRegister(currentPieces[1]))
    {
        std::cerr << convertBitwiseToString(type) << "::arg1::not_matched, given :" << currentPieces[1] << std::endl;
        return false;
    }

    // --------------------------------------------
    //  ARG 2
    // --------------------------------------------

    if(isRegister(currentPieces[2]))
    {
        //std::cout << "Register : " << currentPieces[2] << std::endl;
    }
    else if(isDirectNumerical(currentPieces[2]))
    {
        if(isParserVerbose){ std::cout << "Direct Numerical : " << currentPieces[2] << std::endl; }

        if(!isDirectNumericalInRange(currentPieces[2]))
        {
            std::cerr << "Error: Direct numerical insert is out of range: " << currentPieces[2] << std::endl;
            return false;
        }

        isArg2Register = false;
    }
    else
    {
        std::cerr << convertBitwiseToString(type) << "::arg2::not_matched, given :" << currentPieces[2] << std::endl;
        return false;
    }

    // NOT is the only case where we don't have a 3rd argument. For the sake of ease, we add a fake argument here,
    // Bytegen will ignore the third argument for a NOT construction anyway, but we want to key it as a register
    if(type == NABLA::Bytegen::BitwiseTypes::NOT)
    {
        currentPieces.push_back("r15");
    }

    // --------------------------------------------
    //  ARG 3
    // --------------------------------------------

    if(isRegister(currentPieces[3]))
    {
        if(isParserVerbose){ std::cout << "Register : " << currentPieces[3] << std::endl; }
    }
    else if(isDirectNumerical(currentPieces[3]))
    {
        if(isParserVerbose){ std::cout << "Direct Numerical : " << currentPieces[3] << std::endl; }

        if(!isDirectNumericalInRange(currentPieces[3]))
        {
            std::cerr << "Error: Direct numerical insert is out of range: " << currentPieces[3] << std::endl;
            return false;
        }

        isArg3Register = false;
    }
    else
    {
        std::cerr << convertBitwiseToString(type) << "::arg3::not_matched, given :" << currentPieces[3] << std::endl;
        return false;
    } 

    // Get numerical values of the numbers OR regs
    int16_t arg1 = getNumberFromNumericalOrRegister(currentPieces[1]);
    int16_t arg2 = getNumberFromNumericalOrRegister(currentPieces[2]);
    int16_t arg3 = getNumberFromNumericalOrRegister(currentPieces[3]);

    // Determine how we want to inform the bytegen whats going on 
    NABLA::Bytegen::ArithmaticSetup setup;
    if (isArg2Register && isArg3Register)       { setup = NABLA::Bytegen::ArithmaticSetup::REG_REG; }
    else if (isArg2Register && !isArg3Register) { setup = NABLA::Bytegen::ArithmaticSetup::REG_NUM; }
    else if (isArg3Register && !isArg2Register) { setup = NABLA::Bytegen::ArithmaticSetup::NUM_REG; }
    else                                        { setup = NABLA::Bytegen::ArithmaticSetup::NUM_NUM; }

    // Call to create the instruction and add to current function
    addBytegenInstructionToCurrentFunction(nablaByteGen.createBitwiseInstruction(type, setup, arg1, arg2, arg3));

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_lsh()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "Found stray bitwise instruction. Not currently building a function" << std::endl;
        return false;
    }

    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'LSH' instruction : " << currentLine << std::endl;
        return false;
    }
    
    if(isParserVerbose) { std::cout << "LSH : " << currentLine << std::endl; }

    return bitwise_instruction(NABLA::Bytegen::BitwiseTypes::LSH);
}


// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_rsh()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "Found stray bitwise instruction. Not currently building a function" << std::endl;
        return false;
    }

    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'RSH' instruction : " << currentLine << std::endl;
        return false;
    }
    
    if(isParserVerbose) { std::cout << "RSH : " << currentLine << std::endl; }

    return bitwise_instruction(NABLA::Bytegen::BitwiseTypes::RSH);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_and()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "Found stray bitwise instruction. Not currently building a function" << std::endl;
        return false;
    }

    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'AND' instruction : " << currentLine << std::endl;
        return false;
    }
    
    if(isParserVerbose) { std::cout << "AND : " << currentLine << std::endl; }

    return bitwise_instruction(NABLA::Bytegen::BitwiseTypes::AND);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_or ()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "Found stray bitwise instruction. Not currently building a function" << std::endl;
        return false;
    }

    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'OR' instruction : " << currentLine << std::endl;
        return false;
    }
    
    if(isParserVerbose) { std::cout << "OR : " << currentLine << std::endl; }

    return bitwise_instruction(NABLA::Bytegen::BitwiseTypes::OR);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_xor()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "Found stray bitwise instruction. Not currently building a function" << std::endl;
        return false;
    }

    if(currentPieces.size() != 4)
    {
        std::cerr << "Invalid 'XOR' instruction : " << currentLine << std::endl;
        return false;
    }
    
    if(isParserVerbose) { std::cout << "XOR : " << currentLine << std::endl; }

    return bitwise_instruction(NABLA::Bytegen::BitwiseTypes::XOR);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_not()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "Found stray bitwise instruction. Not currently building a function" << std::endl;
        return false;
    }

    if(currentPieces.size() != 3)
    {
        std::cerr << "Invalid 'NOT' instruction : " << currentLine << std::endl;
        return false;
    }
    
    if(isParserVerbose) { std::cout << "NOT : " << currentLine << std::endl; }

    return bitwise_instruction(NABLA::Bytegen::BitwiseTypes::NOT);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_nop()
{
    if(!isSystemBuildingFunction)
    {
        std::cerr << "Found stray bitwise instruction. Not currently building a function" << std::endl;
        return false;
    }

    if(currentPieces.size() != 1)
    {
        std::cerr << "Invalid 'NOT' instruction : " << currentLine << std::endl;
        return false;
    }

    if(isParserVerbose) { std::cout << "NOP : " << currentLine << std::endl; }


    addBytegenInstructionToCurrentFunction(nablaByteGen.createNopInstruction());

    return true;
}

} // End namespace SOLACE