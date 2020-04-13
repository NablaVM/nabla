/*
    Argument          Parsed            Generated
    ----------------------------------------------
        add             X
        sub             X
        mul             X
        div             X
        add.d           X
        sub.d           X
        mul.d           X
        div.d           X
        bgt   
        bgte  
        blt   
        blte  
        beq   
        bne   
        bgt.d 
        bgte.d
        blt.d 
        blte.d
        beq.d 
        bne.d 
        mov             X
        lda             X
        ldb             X
        stb             X
        push            X
        pop             X
        jmp 
        call
        ret 
        exit    
*/

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
bool instruction_addd();
bool instruction_subd();
bool instruction_divd();
bool instruction_muld();

bool instruction_mov();
bool instruction_lda();
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
    struct Payload
    {
        std::map<std::string, uint32_t>              const_ints;     // Constant integers
        std::map<std::string, uint32_t>              const_doubles;  // Constant doubles
        std::map<std::string, std::vector<uint8_t> > const_strings;  // Constant strings
        std::map<std::string, uint32_t>              labels;         // Labels

        std::vector<std::string> filesParsed;   // Files that have been parsed

        std::string entryPoint;                 // Application entry point

        std::vector<uint8_t> bytes;             // Resulting byte data
    };

    // The final payload
    Payload finalPayload;

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

        MatchCall{ std::regex("^mov$")       , instruction_mov       },
        MatchCall{ std::regex("^lda$")       , instruction_lda       },
        MatchCall{ std::regex("^ldb$")       , instruction_ldb       },
        MatchCall{ std::regex("^stb$")       , instruction_stb       },
        
        MatchCall{ std::regex("^push$")      , instruction_push      },
        MatchCall{ std::regex("^pop$")       , instruction_pop      },

        MatchCall{ std::regex("^jmp$")       , instruction_jmp       },
        MatchCall{ std::regex("^call$")      , instruction_call      },
        MatchCall{ std::regex("^ret$")       , instruction_return    },

        MatchCall{ std::regex("^exit$")      , instruction_exit      },
        MatchCall{ std::regex("^\\.[a-z]+$") , instruction_directive },
        MatchCall{ std::regex("^\\<[a-zA-Z0-9_]+:$") , instruction_create_function },
        MatchCall{ std::regex("^\\>$") , instruction_end_function },

        
    };

    constexpr char UNDEFINED_ENTRY_POINT[] = "___UNDEFINED__ENTRY__POINT___";

    enum class ArithmaticTypes
    {
        ADD  = 0x00,
        MUL  = 0x01,
        DIV  = 0x02,
        SUB  = 0x03,
        SUBD = 0x04,
        DIVD = 0x05,
        ADDD = 0x06,
        MULD = 0x07
    };

    enum class BranchTypes
    {  
        BGT   = 0x01, 
        BGTE  = 0x02, 
        BLT   = 0x03, 
        BLTE  = 0x04, 
        BEQ   = 0x05, 
        BNE   = 0x06, 
        BGTD  = 0x07,
        BGTED = 0x08,
        BLTD  = 0x09,
        BLTED = 0x0A,
        BEQD  = 0x0B,
        BNED  = 0x0C,
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

            if(currentPieces.size() > 0)
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

inline static bool isStackOffsetInRange(std::string piece)
{
    std::string str = piece.substr(1, piece.size()-5);

    int64_t n = std::stoi(str);

    return ( n < 4294967295 );
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

inline static bool isDirectNumericalInRange(std::string numerical)
{
    std::string str = numerical.substr(1, numerical.size());

    int n = std::stoi(str);

    return ( n < 32767 && n > -32768 );
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
        case ArithmaticTypes::ADDD: return "ADDD";
        case ArithmaticTypes::MUL : return "MUL";
        case ArithmaticTypes::MULD: return "MULD";
        case ArithmaticTypes::DIV : return "DIV";
        case ArithmaticTypes::DIVD: return "DIVD";
        case ArithmaticTypes::SUB : return "SUB";
        case ArithmaticTypes::SUBD: return "SUBD";
        default:                    return "UNKNOWN"; // Keep that compiler happy.
    }
}

// -----------------------------------------------
// Parsed, not complete
// -----------------------------------------------

inline static bool arithmatic_instruction(ArithmaticTypes type)
{

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
        }
        else
        {
            std::cout << convertArithToString(type) << "::arg1::not_matched, given :" << currentPieces[1] << std::endl;
            return false;
        }

        bool argumentTwoFound   = false;
        bool argumentThreeFound = false;

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
            std::cout << convertArithToString(type) << "::arg2::not_matched, given :" << currentPieces[2] << std::endl;
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

            if(!isDirectNumericalInRange(currentPieces[3]))
            {
                std::cerr << "Error: Direct numerical insert is out of range: " << currentPieces[3] << std::endl;
                return false;
            }

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
            std::cout << convertArithToString(type) << "::arg3::not_matched, given :" << currentPieces[3] << std::endl;
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
            std::cout << convertArithToString(type) << "::arg1::register::" << currentPieces[1] << std::endl;
        }
        else
        {
            std::cout << convertArithToString(type) << "::arg1::not_matched, given :" << currentPieces[1] << std::endl;
            return false;
        }

        // --------------------------------------------
        //  ARG 2
        // --------------------------------------------

        if(isRegister(currentPieces[2]))
        {
            std::cout << "Double Register : " << currentPieces[2] << std::endl;
        }
        else
        {
            std::cout << convertArithToString(type) << "::arg2::not_matched, given :" << currentPieces[2] << std::endl;
            return false;
        }

        // --------------------------------------------
        //  ARG 3
        // --------------------------------------------

        if(isRegister(currentPieces[3]))
        {
            std::cout << "Register : " << currentPieces[3] << std::endl;
        }
        else
        {
            std::cout << convertArithToString(type) << "::arg3::not_matched, given :" << currentPieces[3] << std::endl;
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
        std::cerr << "Invalid 'add' instruction : " << currentLine << std::endl;
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
        std::cerr << "Invalid 'sub' instruction : " << currentLine << std::endl;
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
        std::cerr << "Invalid 'div' instruction : " << currentLine << std::endl;
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
        std::cerr << "Invalid 'mul' instruction : " << currentLine << std::endl;
        return false;
    }
    
    return arithmatic_instruction(ArithmaticTypes::MUL);
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
    
    return arithmatic_instruction(ArithmaticTypes::ADDD);
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
    
    return arithmatic_instruction(ArithmaticTypes::SUBD);
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
    
    return arithmatic_instruction(ArithmaticTypes::DIVD);
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
    
    return arithmatic_instruction(ArithmaticTypes::MULD);
}

// -----------------------------------------------
// Parsed, not complete
// -----------------------------------------------

bool instruction_mov()
{
    std::cout << "mov : " << currentLine << std::endl;

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

    if(!isRegister(currentPieces[2]))
    {
        std::cerr << "Argument 2 of 'mov' must be a register" << std::endl;
        return false;
    }

    // Both are confirmed registers!

    return true;
}

// -----------------------------------------------
// Parsed, not complete
// -----------------------------------------------

bool instruction_lda()
{
    std::cout << "lda : " << currentLine << std::endl;
    
    if(currentPieces.size() != 3)
    {
        std::cerr << "Invalid 'lda' instruction : " << currentLine << std::endl;
        return false;
    }

    // Check if arg1 is a register
    if(!isRegister(currentPieces[1]))
    {
        std::cerr << "Error: First argument of 'lda' must be a register, but [" << currentPieces[1] << "] was given" << std::endl;
        return false;
    }

    // Check if arg2 is a register
    if (isReferencedConstant(currentPieces[2]))
    {
        std::string constantName = currentPieces[2].substr(1, currentPieces[2].size());

        std::cout << "Argument 2 is referenced constant : " << constantName;

        // Is it an int ?
        if(isConstIntInPayload(constantName))
        {
            std::cout << " -> constant_int " << std::endl;

        }

        // Is it a double ?
        else if (isConstDoubleInPayload(constantName))
        {
            std::cout << " -> constant_double " << std::endl;

        }
        
        // Is it a string ?
        else if (isConstStringInPayload(constantName))
        {
            std::cout << " -> constant_string " << std::endl;

        }

        // IT ISN'T ANYTHING ! UGH!
        else
        {
            std::cout << " ERROR - Unable to locate given constant" << std::endl;
            return false;
        }
    }

    // Check if its a global stack pointer
    else if (isOffsetGlobalStackpointer(currentPieces[2]))
    {
        std::cout << "Argument 2 -> global_stack_pointer " << std::endl;

    }

    // Check if its a local stack pointer
    else if (isOffsetLocalStackpointer(currentPieces[2]))
    {
        std::cout << "Argument 2 -> local_stack_pointer " << std::endl;

    }
    else
    {
        std::cout << "'lda' argument 2 not matched" << std::endl;
        return false;
    }

    return true;
}

// -----------------------------------------------
// Parsed, not complete
// -----------------------------------------------

bool instruction_stb()
{
    std::cout << "stb : " << currentLine << std::endl;

    if(currentPieces.size() != 3)
    {
        std::cerr << "Invalid 'stb' instruction : " << currentLine << std::endl;
        return false;
    }

    // Check argument 1 
    if(isOffsetGlobalStackpointer(currentPieces[1]))
    {
        std::cout << "Argument 1 is global spo : " << currentPieces[1] << std::endl;
    }
    else if (isOffsetLocalStackpointer(currentPieces[1]))
    {
        std::cout << "Argument 1 is local spo : " << currentPieces[1] << std::endl;
    }
    else
    {
        std::cerr << "'stb' argument 1 must be a stack pointer offset, got : " << currentPieces[1] << " instread" << std::endl;
        return false;
    }
    
    if(!isRegister(currentPieces[2]))
    {
        std::cerr << "'stb' argument 2 must be a register, got : " << currentPieces[2] << " instead" << std::endl;
        return false;
    }

    return true;
}

// -----------------------------------------------
// Parsed, not complete
// -----------------------------------------------

bool instruction_ldb()
{
    std::cout << "ldb : " << currentLine << std::endl;

    if(currentPieces.size() != 3)
    {
        std::cerr << "Invalid 'ldb' instruction : " << currentLine << std::endl;
        return false;
    }

    if(!isRegister(currentPieces[1]))
    {
        std::cerr << "Error: First argument of 'ldb' must be a register, but [" << currentPieces[1] << "] was given" << std::endl;
        return false;
    }

    if(isRegister(currentPieces[2]))
    {
        std::cout << "Argument 2 is register : " << currentPieces[2] << std::endl;

    }

    // Check if its a referenced constant, and if it is try to find it
    else if (isReferencedConstant(currentPieces[2]))
    {
        std::string constantName = currentPieces[2].substr(1, currentPieces[2].size());

        std::cout << "Argument 2 is referenced constant : " << constantName;

        // Is it an int ?
        if(isConstIntInPayload(constantName))
        {
            std::cout << " -> constant_int " << std::endl;

        }

        // Is it a double ?
        else if (isConstDoubleInPayload(constantName))
        {
            std::cout << " -> constant_double " << std::endl;

        }
        
        // Is it a string ?
        else if (isConstStringInPayload(constantName))
        {
            std::cout << " -> constant_string " << std::endl;

        }

        // IT ISN'T ANYTHING ! UGH!
        else
        {
            std::cout << " ERROR - Unable to locate given constant" << std::endl;
            return false;
        }
    }

    // Check if its a global stack pointer
    else if (isOffsetGlobalStackpointer(currentPieces[2]))
    {
        std::cout << "Argument 2 -> global_stack_pointer " << std::endl;

    }

    // Check if its a local stack pointer
    else if (isOffsetLocalStackpointer(currentPieces[2]))
    {
        std::cout << "Argument 2 -> local_stack_pointer " << std::endl;

    }
    else
    {
        std::cout << "'ldb' argument 2 must be constant or stack offset" << std::endl;
        return false;
    }

    return true;
}

// -----------------------------------------------
// Parsed, not complete
// -----------------------------------------------

bool instruction_push()
{
    std::cout << "push : " << currentLine << std::endl;

    if(!currentPieces.size() == 3)
    {
        std::cerr << "Invalid 'push' instruction : " << currentLine << std::endl;
        return false; 
    }

    // Argument 2
    if(isDirectLocalStackPointer(currentPieces[1]))
    {
        std::cout << "Argument 1 is a local stack pointer " << std::endl;
    }
    else if (isDirectGlobalStackPointer(currentPieces[1]))
    {
        std::cout << "Argument 1 is a global stack pointer " << std::endl;
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
    }

    return true;
}

// -----------------------------------------------
// Parsed, not complete
// -----------------------------------------------

bool instruction_pop()
{
    std::cout << "pop : " << currentLine << std::endl;

    if(!currentPieces.size() == 3)
    {
        std::cerr << "Invalid 'pop' instruction : " << currentLine << std::endl;
        return false; 
    }

    // Argument 2
    if(isDirectLocalStackPointer(currentPieces[1]))
    {
        std::cout << "Argument 1 is a local stack pointer " << std::endl;
    }
    else if (isDirectGlobalStackPointer(currentPieces[1]))
    {
        std::cout << "Argument 1 is a global stack pointer " << std::endl;
    }
    else
    {
        std::cerr << "'pop' instruction argument 1 must be a global or local stack pointer (not an offset)" << std::endl;
        return false;
    }
    
    // Argument 2
    if(!isRegister(currentPieces[2]))
    {
        std::cout << "'pop' instruction argument 2 must be a register" << std::endl;
    }

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static std::string convertBranchToString(BranchTypes type)
{
    switch(type)
    {
        case BranchTypes::BGT  : return "BGT  "; 
        case BranchTypes::BGTE : return "BGTE "; 
        case BranchTypes::BLT  : return "BLT  "; 
        case BranchTypes::BLTE : return "BLTE "; 
        case BranchTypes::BEQ  : return "BEQ  "; 
        case BranchTypes::BNE  : return "BNE  "; 
        case BranchTypes::BGTD : return "BGTD ";
        case BranchTypes::BGTED: return "BGTED";
        case BranchTypes::BLTD : return "BLTD ";
        case BranchTypes::BLTED: return "BLTED";
        case BranchTypes::BEQD : return "BEQD ";
        case BranchTypes::BNED : return "BNED ";
        default:                 return "UNKNOWN"; // Keep that compiler happy.
    }
}

inline bool branch_instruction(BranchTypes type)
{
    std::cout << "Branch : " << convertBranchToString(type) << ": " << currentLine << std::endl;


    if(static_cast<unsigned>(type) <= 0x06)
    {
        std::cout << "Regular, non-double branch" << std::endl;
    }
    else
    {
        std::cout << "Not regular, double branch" << std::endl;
    }


    std::cerr << "Returning error because this aint done dood" << std::endl;
    return false;
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

    return branch_instruction(BranchTypes::BGT);
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

    return branch_instruction(BranchTypes::BGT);
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

    return branch_instruction(BranchTypes::BGTE);
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

    return branch_instruction(BranchTypes::BLTE);
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

    return branch_instruction(BranchTypes::BEQ);
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

    return branch_instruction(BranchTypes::BNE);
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

    return branch_instruction(BranchTypes::BGTD);
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

    return branch_instruction(BranchTypes::BLT);
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

    return branch_instruction(BranchTypes::BGTED);
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

    return branch_instruction(BranchTypes::BLTED);
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

    return branch_instruction(BranchTypes::BEQD);
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

    return branch_instruction(BranchTypes::BNED);
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_jmp()
{
    std::cout << "jmp : " << currentLine << std::endl;

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_call()
{
    std::cout << "call : " << currentLine << std::endl;

    // Put return address in sys0
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

        // Ensure that we haven't had it defined yet
        if(isConstStringInPayload(currentPieces[1]))
        {
            std::cerr << "Constant .string " << currentPieces[1] << " previously defined with value."<< std::endl;
            return false;
        }

        // Get the destination vector ready
        finalPayload.const_strings[currentPieces[1]] = std::vector<uint8_t>();

        // Put the chars in the array
        for(auto& i : str)
        {
            finalPayload.const_strings[currentPieces[1]].push_back(static_cast<uint8_t>(i));
        }
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

bool instruction_create_function()
{
    std::cout << "Create function: " << currentLine << std::endl;

    return true;
}

// -----------------------------------------------
//
// -----------------------------------------------

bool instruction_end_function()
{
    std::cout << "End function: " << currentLine << std::endl;

    return true;
}


} // End namespace SOLACE