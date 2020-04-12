#include "solace.hpp"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <regex>
#include <functional>
#include <map>

namespace SOLACE
{

bool instruction_add();
bool instruction_sub();
bool instruction_div();
bool instruction_mul();
bool instruction_dadd();
bool instruction_dsub();
bool instruction_ddiv();
bool instruction_dmul();
bool instruction_mov();
bool instruction_dmov();
bool instruction_ldw();
bool instruction_stw();
bool instruction_ldc();
bool instruction_dldw();
bool instruction_dstw();
bool instruction_dldc();
bool instruction_bgt();
bool instruction_blt();
bool instruction_bgte();
bool instruction_blte();
bool instruction_beq();
bool instruction_bne();
bool instruction_jmp();
bool instruction_return();
bool instruction_exit();
bool instruction_directive();
bool instruction_create_label();

namespace 
{
    struct Payload
    {
        std::map<std::string, uint32_t> const_ints;     // Name / Values. 
        std::map<std::string, uint32_t> const_strings;
        std::map<std::string, uint32_t> const_doubles;

        std::map<std::string, uint32_t> labels;

        std::vector<std::string> filesParsed;

        std::string entryPoint;

        std::vector<uint8_t> bytes;                     // Resulting byte codes
    };

    Payload finalPayload;

    std::string currentLine;
    std::vector<std::string> currentPieces;

    struct MatchCall
    {
        std::regex reg;
        std::function<bool()> call;
    };

    std::vector<MatchCall> parserMethods = {
        MatchCall{ std::regex("^add$")       , instruction_add       },
        MatchCall{ std::regex("^dadd$")      , instruction_dadd      },
        MatchCall{ std::regex("^sub$")       , instruction_sub       },
        MatchCall{ std::regex("^dsub$")      , instruction_dsub      },
        MatchCall{ std::regex("^mul$")       , instruction_mul       },
        MatchCall{ std::regex("^dmul$")      , instruction_dmul      },
        MatchCall{ std::regex("^div$")       , instruction_div       },
        MatchCall{ std::regex("^ddiv$")      , instruction_ddiv      },
        MatchCall{ std::regex("^mov$")       , instruction_mov       },
        MatchCall{ std::regex("^dmov$")      , instruction_dmov       },
        MatchCall{ std::regex("^ldw$")       , instruction_ldw       },
        MatchCall{ std::regex("^stw$")       , instruction_stw       },
        MatchCall{ std::regex("^ldc$")       , instruction_ldc       },
        MatchCall{ std::regex("^dldw$")      , instruction_dldw      },
        MatchCall{ std::regex("^dstw$")      , instruction_dstw      },
        MatchCall{ std::regex("^dldc$")      , instruction_dldc      },
        MatchCall{ std::regex("^bgt$")       , instruction_bgt       },
        MatchCall{ std::regex("^blt$")       , instruction_blt       },
        MatchCall{ std::regex("^bgte$")      , instruction_bgte      },
        MatchCall{ std::regex("^blte$")      , instruction_blte      },
        MatchCall{ std::regex("^beq$")       , instruction_beq       },
        MatchCall{ std::regex("^bne$")       , instruction_bne       },
        MatchCall{ std::regex("^jmp$")       , instruction_jmp       },
        MatchCall{ std::regex("^ret$")       , instruction_return    },
        MatchCall{ std::regex("^exit$")      , instruction_exit      },
        MatchCall{ std::regex("^\\.[a-z]+$") , instruction_directive },
        MatchCall{ std::regex("[a-zA-Z0-9_]+:$") , instruction_create_label },
    };

    constexpr char UNDEFINED_ENTRY_POINT[] = "___UNDEFINED__ENTRY__POINT___";

    enum class ArithmaticTypes
    {
        ADD  = 0x00,
        MUL  = 0x01,
        DIV  = 0x02,
        SUB  = 0x03,
        DSUB = 0x04,
        DDIV = 0x05,
        DADD = 0x06,
        DMUL = 0x07
    };
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

inline static std::string &ltrim (std::string &line)
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

inline static bool parseFile(std::string file)
{
    std::ifstream ifs(file);

    if(!ifs.is_open())
    {
        std::cout << "Unable to open file : " <<file << std::endl;
        return false;
    }

    while(std::getline(ifs, currentLine))
    {
        currentLine = currentLine.substr(0, currentLine.find(";", 0));
        currentLine = ltrim(currentLine);

        if(currentLine.length() > 0)
        {
            currentPieces = chunkLine(currentLine);

            if(currentPieces.size() > 1)
            {
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
        }
    }

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool ParseAsm(std::string asmFile, std::vector<uint8_t> &bytes)
{
    // Mark the entry point as undefined to start
    finalPayload.entryPoint = UNDEFINED_ENTRY_POINT;

    // Mark the file as parsed before anything in an attempt to reduce depends loops =]
    finalPayload.filesParsed.push_back(asmFile);

    // Parse the first file - Include directives will drive the parsing
    // of required files
    if(!parseFile(asmFile))
    {
        return false;
    }

    bytes = finalPayload.bytes;

    return true;
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
    return std::regex_match(name, std::regex("^[0-9]+$"));
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
    return std::regex_match(piece, std::regex("^r{1}([0-9]|1[015])$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isDoubleRegister(std::string piece)
{
    return std::regex_match(piece, std::regex("^d{1}[0-7]$"));
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

inline static bool isDirectStackPointer(std::string piece)
{
    return std::regex_match(piece, std::regex("^sp$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isStackPointerOffset(std::string piece)
{
    return std::regex_match(piece, std::regex("^\\$[0-9]+\\(sp\\)$"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isDirectNumerical(std::string piece)
{
    return std::regex_match(piece, std::regex("^\\$[0-9]+$"));
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
//
// -----------------------------------------------

inline static bool isConstIntInPayload(std::string name)
{
    return (finalPayload.const_ints.find(name) != finalPayload.const_ints.end());
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isConstDoubleInPayload(std::string name)
{
    return (finalPayload.const_doubles.find(name) != finalPayload.const_doubles.end());
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isConstStringInPayload(std::string name)
{
    return (finalPayload.const_strings.find(name) != finalPayload.const_strings.end());
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isLabelInPayload(std::string name)
{
    return (finalPayload.labels.find(name) != finalPayload.labels.end());
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool wasFileParsed(std::string name)
{
    return (std::find(finalPayload.filesParsed.begin(), finalPayload.filesParsed.end(), name) != finalPayload.filesParsed.end());
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static std::string convertArithToString(ArithmaticTypes type)
{
    switch(type)
    {
        case ArithmaticTypes::ADD : return "ADD";
        case ArithmaticTypes::DADD: return "DADD";
        case ArithmaticTypes::MUL : return "MUL";
        case ArithmaticTypes::DMUL: return "DMUL";
        case ArithmaticTypes::DIV : return "DIV";
        case ArithmaticTypes::DDIV: return "DDIV";
        case ArithmaticTypes::SUB : return "SUB";
        case ArithmaticTypes::DSUB: return "DSUB";
        default:                    return "UNKNOWN"; // Keep that compiler happy.
    }
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool arithmatic_instruction(ArithmaticTypes type)
{
    bool argumentOneFound   = false;
    bool argumentTwoFound   = false;
    bool argumentThreeFound = false;

    // These are the non-double commands
    if(static_cast<unsigned>(type) <= 0x03)
    {
        // --------------------------------------------
        //  ARG 1
        // --------------------------------------------

        // Check if is register
        if(isRegister(currentPieces[1]))
        {
            std::cout << convertArithToString(type) << "::arg1::register::" << currentPieces[1] << std::endl;

            argumentOneFound = true;
        }

        // ADDs can have a stack pointer in the first bit, so we need to check that
        if(ArithmaticTypes::ADD == type && !argumentOneFound)
        {
            if(isStackPointerOffset(currentPieces[1]))
            {
                std::cout << convertArithToString(type) << "::arg1::stack_pointer::" << currentPieces[1] << std::endl;
                argumentOneFound = true;
            }
        }

        if(!argumentOneFound)
        {
            std::cout << convertArithToString(type) << "::arg1::not_matched, given :" << currentPieces[1] << std::endl;
            return false;
        }

        // --------------------------------------------
        //  ARG 2
        // --------------------------------------------

        if(isRegister(currentPieces[2]))
        {
            std::cout << "Register : " << currentPieces[2] << std::endl;
            argumentTwoFound = true;
        }
        else if(isDirectNumerical(currentPieces[2]))
        {
            std::cout << "Direct Numerical : " << currentPieces[2] << std::endl; 
            argumentTwoFound = true;
        }
        else if(isStackPointerOffset(currentPieces[2]))
        {
            std::cout << "Stack pointer offset : " << currentPieces[2] << std::endl; 
            argumentTwoFound = true;
        }
        else if(isReferencedConstant(currentPieces[2]))
        {
            std::cout << "Referenced Constant : " << currentPieces[2] << std::endl;

            std::string constantName = currentPieces[2].substr(1, currentPieces[2].size());

            if(!isConstIntInPayload(constantName))
            {
                std::cerr << "Given 'int' constant has not been set : " << constantName << std::endl;
                return false;
            }
            argumentTwoFound = true;
        }

        if(!argumentTwoFound)
        {
            std::cout << convertArithToString(type) << "::arg2::not_matched, given :" << currentPieces[1] << std::endl;
            return false;
        }

        // --------------------------------------------
        //  ARG 3
        // --------------------------------------------

        if(isRegister(currentPieces[3]))
        {
            std::cout << "Register : " << currentPieces[3] << std::endl;
            argumentThreeFound = true;
        }
        else if(isDirectNumerical(currentPieces[3]))
        {
            std::cout << "Direct Numerical : " << currentPieces[3] << std::endl;
            argumentThreeFound = true;
        }
        else if(isStackPointerOffset(currentPieces[3]))
        {
            std::cout << "Stack pointer offset : " << currentPieces[3] << std::endl; 
            argumentThreeFound = true;
        }
        else if(isReferencedConstant(currentPieces[3]))
        {
            std::cout << "Referenced Constant : " << currentPieces[3] << std::endl;

            std::string constantName = currentPieces[3].substr(1, currentPieces[3].size());

            if(!isConstIntInPayload(constantName))
            {
                std::cerr << "Given 'int' constant has not been set : " << constantName << std::endl;
                return false;
            }
            argumentThreeFound = true;
        }

        if(!argumentThreeFound)
        {
            std::cout << convertArithToString(type) << "::arg3::not_matched, given :" << currentPieces[1] << std::endl;
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
        if(isDoubleRegister(currentPieces[1]))
        {
            std::cout << convertArithToString(type) << "::arg1::double_register::" << currentPieces[1] << std::endl;

            argumentOneFound = true;
        }

        if(!argumentOneFound)
        {
            std::cout << convertArithToString(type) << "::arg1::not_matched, given :" << currentPieces[1] << std::endl;
            return false;
        }

        // --------------------------------------------
        //  ARG 2
        // --------------------------------------------

        if(isDoubleRegister(currentPieces[2]))
        {
            std::cout << "Double Register : " << currentPieces[2] << std::endl;
            argumentTwoFound = true;
        }
        else if(isDirectNumericalDouble(currentPieces[2]))
        {
            std::cout << "Direct Numerical Double: " << currentPieces[2] << std::endl; 
            argumentTwoFound = true;
        }
        else if(isReferencedConstant(currentPieces[2]))
        {
            std::cout << "Referenced Constant : " << currentPieces[2] << std::endl;

            std::string constantName = currentPieces[2].substr(1, currentPieces[2].size());

            if(!isConstDoubleInPayload(constantName))
            {
                std::cerr << "Given 'double' constant has not been set : " << constantName << std::endl;
                return false;
            }
            argumentTwoFound = true;
        }

        if(!argumentTwoFound)
        {
            std::cout << convertArithToString(type) << "::arg2::not_matched, given :" << currentPieces[1] << std::endl;
            return false;
        }

        // --------------------------------------------
        //  ARG 3
        // --------------------------------------------

        if(isDoubleRegister(currentPieces[3]))
        {
            std::cout << "Double Register : " << currentPieces[3] << std::endl;
            argumentThreeFound = true;
        }
        else if(isDirectNumericalDouble(currentPieces[3]))
        {
            std::cout << "Direct Numerical Double: " << currentPieces[3] << std::endl;
            argumentThreeFound = true;
        }
        else if(isReferencedConstant(currentPieces[3]))
        {
            std::cout << "Referenced Constant : " << currentPieces[3] << std::endl;

            std::string constantName = currentPieces[3].substr(1, currentPieces[3].size());

            if(!isConstDoubleInPayload(constantName))
            {
                std::cerr << "Given 'double' constant has not been set : " << constantName << std::endl;
                return false;
            }
            argumentThreeFound = true;
        }

        if(!argumentThreeFound)
        {
            std::cout << convertArithToString(type) << "::arg3::not_matched, given :" << currentPieces[1] << std::endl;
            return false;
        }
    }
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_add()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Incomplete 'add' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(ArithmaticTypes::ADD);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_sub()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Incomplete 'sub' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(ArithmaticTypes::SUB);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_div()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Incomplete 'div' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(ArithmaticTypes::DIV);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_mul()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Incomplete 'mul' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(ArithmaticTypes::MUL);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_dadd()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Incomplete 'dadd' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(ArithmaticTypes::DADD);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_dsub()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Incomplete 'dsub' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(ArithmaticTypes::DSUB);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_ddiv()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Incomplete 'ddiv' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(ArithmaticTypes::DDIV);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_dmul()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Incomplete 'dmul' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(ArithmaticTypes::DMUL);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_mov()
{
#warning START WORKING HERE
    std::cout << "mov (NYD): " << currentLine << std::endl;
    return false;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_dmov()
{
#warning OR START WORKING HERE
    std::cout << "dmov (NYD): " << currentLine << std::endl;
    return false;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_ldw()
{
    std::cout << "ldw : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_stw()
{
    std::cout << "stw : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_ldc()
{
    std::cout << "ldc : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_dldw()
{
    std::cout << "dldw : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_dstw()
{
    std::cout << "dstw : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_dldc()
{
    std::cout << "dldc : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_bgt()
{
    std::cout << "bgt : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_blt()
{
    std::cout << "blt : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_bgte()
{
    std::cout << "bgte : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_blte()
{
    std::cout << "blte : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_beq()
{
    std::cout << "beq : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_bne()
{
    std::cout << "bne : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_jmp()
{
    std::cout << "jmp : " << currentLine << std::endl;

    // Put return address in sys1 - Consider adding a 'call'
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_return()
{
    std::cout << "return : " << currentLine << std::endl;

    // Pull address from sys1
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_exit()
{
    std::cout << "exit : " << currentLine << std::endl;
    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_directive()
{
    std::cout << "Directive: " << currentLine << std::endl;

    if(currentPieces.size() < 2)
    {
        std::cerr << "Unknown potential directive : " << currentLine << std::endl;
        return false;
    }

    // ----------------------------------------------------------------------
    //  Set entry point
    // ----------------------------------------------------------------------
    if(std::regex_match(currentPieces[0], std::regex("^\\.init$")))
    {
        std::cout << "\tinit: " << currentLine << std::endl;

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

        if(isLabelInPayload(currentPieces[1]))
        {
            std::cerr << ".init [" << currentPieces[1] << "] already parsed into solace payload." << std::endl;
            return false;
        }

        finalPayload.entryPoint = currentPieces[1];
    }
    // ----------------------------------------------------------------------
    //  Set .file
    // ----------------------------------------------------------------------
    else if (std::regex_match(currentPieces[0], std::regex("^\\.file$")))
    {
        std::cout << "\tfile: " << currentLine[1] << std::endl;
    }
    // ----------------------------------------------------------------------
    //  Create a .string constant
    // ----------------------------------------------------------------------
    else if (std::regex_match(currentPieces[0], std::regex("^\\.string$"))) 
    {
        std::cout << "\tstring: " << currentLine << std::endl;
    }
    // ----------------------------------------------------------------------
    //  Create a .int constant
    // ----------------------------------------------------------------------
    else if (std::regex_match(currentPieces[0], std::regex("^\\.int$"))) 
    {
        std::cout << "\tint: " << currentLine << std::endl;

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
        if(isConstIntInPayload(currentPieces[1]))
        {
            std::cerr << "Constant .int " << currentPieces[1] << " previously defined with value : " << finalPayload.const_ints[currentPieces[1]] << std::endl;
            return false;
        }

        // Get the int
        int64_t givenInt = std::stoi(currentPieces[2]);

        // Store it
        finalPayload.const_ints[currentPieces[1]] = static_cast<uint32_t>(givenInt);
    }
    // ----------------------------------------------------------------------
    //  Create a .double constant
    // ----------------------------------------------------------------------
    else if (std::regex_match(currentPieces[0], std::regex("^\\.double$"))) 
    {
        std::cout << "\tdouble: " << currentLine << std::endl;

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
        if(isConstDoubleInPayload(currentPieces[1]))
        {
            std::cerr << "Constant .double " << currentPieces[1] << " previously defined with value : " << finalPayload.const_doubles[currentPieces[1]] << std::endl;
            return false;
        }

        // Get the int
        double divenDouble = std::stod(currentPieces[2]);

#warning this double to uint32_t needs to be evaluated
        // Store it
        finalPayload.const_doubles[currentPieces[1]] = static_cast<uint32_t>(divenDouble + 0.5);
    }
    // ----------------------------------------------------------------------
    //  Include a file
    // ----------------------------------------------------------------------
    else if (std::regex_match(currentPieces[0], std::regex("^\\.include$"))) 
    {
        std::cout << "\tinclude: " << currentLine << std::endl;

        // Check if it has been parsed before
        if(wasFileParsed(currentPieces[1]))
        {
            std::cout << "WARN: File <" << currentPieces[1] << "> already marked as parsed. Skipping" << std::endl;
            return true;
        }

#warning This hasn't been tested. 
        parseFile(currentPieces[1]);

        finalPayload.filesParsed.push_back(currentPieces[1]);
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

bool instruction_create_label()
{
    std::cout << "Create Label: " << currentLine << std::endl;

    return true;
}

} // End namespace SOLACE