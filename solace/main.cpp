#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <regex>
#include <functional>
#include <map>

void instruction_add();
void instruction_sub();
void instruction_div();
void instruction_mul();
void instruction_dadd();
void instruction_dsub();
void instruction_ddiv();
void instruction_dmul();
void instruction_mov();
void instruction_ldw();
void instruction_stw();
void instruction_ldc();
void instruction_dldw();
void instruction_dstw();
void instruction_dldc();
void instruction_bgt();
void instruction_blt();
void instruction_bgte();
void instruction_blte();
void instruction_beq();
void instruction_bne();
void instruction_jmp();
void instruction_return();
void instruction_exit();
void instruction_directive();
void instruction_create_label();

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
        std::function<void()> call;
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

    enum ArithmaticTypes
    {
        ADD ,
        DADD,
        MUL ,
        DMUL,
        DIV ,
        DDIV,
        SUB ,
        DSUB
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

inline static void parseFile(std::string file)
{
    std::ifstream ifs(file);

    if(!ifs.is_open())
    {
        std::cout << "Unable to open file : " <<file << std::endl;
        exit(EXIT_FAILURE);
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
                        i.call();
                        found = true;
                    }
                }

                if(!found)
                {
                    std::cerr << "Unknown instruction(s) : " << currentLine << std::endl;
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
}

// -----------------------------------------------
//
// -----------------------------------------------

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

    // Mark the entry point as undefined to start
    finalPayload.entryPoint = UNDEFINED_ENTRY_POINT;

    // Mark the file as parsed before anything in an attempt to reduce depends loops =]
    finalPayload.filesParsed.push_back(argv[1]);

    // Parse the first file
    parseFile(argv[1]);
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

inline static bool isDirectNumerical(std::string piece)
{
    return std::regex_match(piece, std::regex("^\\$[0-9]+"));
}

// -----------------------------------------------
//
// -----------------------------------------------

inline static bool isReferencedConstant(std::string piece)
{
    return std::regex_match(piece, std::regex("^\\&[a-zA-Z_0-9]+"));
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

inline static void arithmatic_instruction(ArithmaticTypes type)
{

}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_add()
{
    if(currentPieces.size() != 4)
    {
        std::cerr << "Incomplete instruction : " << currentLine << std::endl;
        exit(EXIT_FAILURE);
    }
    
    // Check ARG 1
    if(isRegister(currentPieces[1]))
    {
        std::cout << "Register : " << currentPieces[1] << std::endl;
    }
    else if(isDirectStackPointer(currentPieces[1]))
    {
        std::cout << "Direct Stack Pointer : " << currentPieces[1] << std::endl;
    }
    else
    {
        std::cerr << "Unacceptable Arg1 for 'add' instruction : " << currentLine << std::endl;
        exit(EXIT_FAILURE);
    }

    // Check ARG 2
    if(isRegister(currentPieces[2]))
    {
        std::cout << "Register : " << currentPieces[2] << std::endl;
    }
    else if(isDirectStackPointer(currentPieces[2]))
    {
        std::cout << "Direct Stack Pointer : " << currentPieces[2] << std::endl;
    }
    else if(isDirectNumerical(currentPieces[2]))
    {
        std::cout << "Direct Numerical : " << currentPieces[2] << std::endl; 
    }
    else if(isReferencedConstant(currentPieces[2]))
    {
        std::cout << "Referenced Constant : " << currentPieces[2] << std::endl;

        std::string constantName = currentPieces[2].substr(1, currentPieces[2].size());

        if(!isConstIntInPayload(constantName))
        {
            std::cerr << "Given constant has not been set : " << constantName << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "Unacceptable Arg2 for 'add' instruction : " << currentLine << std::endl;
        exit(EXIT_FAILURE);
    }

    // Check ARG 3
    if(isRegister(currentPieces[3]))
    {
        std::cout << "Register : " << currentPieces[3] << std::endl;
    }
    else if(isDirectNumerical(currentPieces[3]))
    {
        std::cout << "Direct Numerical : " << currentPieces[3] << std::endl;
    }
    else if(isReferencedConstant(currentPieces[3]))
    {
        std::cout << "Referenced Constant : " << currentPieces[3] << std::endl;

        std::string constantName = currentPieces[3].substr(1, currentPieces[3].size());

        if(!isConstIntInPayload(constantName))
        {
            std::cerr << "Given constant has not been set : " << constantName << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else
    {
        std::cerr << "Unacceptable Arg3 for 'add' instruction : " << currentLine << std::endl;
        exit(EXIT_FAILURE);
    }
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_sub()
{
    std::cout << "sub : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_div()
{
    std::cout << "div : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_mul()
{
    std::cout << "mul : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_dadd()
{
    std::cout << "dadd : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_dsub()
{
    std::cout << "dsub : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_ddiv()
{
    std::cout << "ddiv : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_dmul()
{
    std::cout << "dmul : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_mov()
{
    std::cout << "mov : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_ldw()
{
    std::cout << "ldw : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_stw()
{
    std::cout << "stw : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_ldc()
{
    std::cout << "ldc : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_dldw()
{
    std::cout << "dldw : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_dstw()
{
    std::cout << "dstw : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_dldc()
{
    std::cout << "dldc : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_bgt()
{
    std::cout << "bgt : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_blt()
{
    std::cout << "blt : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_bgte()
{
    std::cout << "bgte : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_blte()
{
    std::cout << "blte : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_beq()
{
    std::cout << "beq : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_bne()
{
    std::cout << "bne : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_jmp()
{
    std::cout << "jmp : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_return()
{
    std::cout << "return : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_exit()
{
    std::cout << "exit : " << currentLine << std::endl;
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_directive()
{
    std::cout << "Directive: " << currentLine << std::endl;

    if(currentPieces.size() < 2)
    {
        std::cerr << "Unknown potential directive : " << currentLine << std::endl;
        exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
        }

        if(finalPayload.entryPoint != UNDEFINED_ENTRY_POINT)
        {
            std::cerr << "Entry point already defined as [" 
                      << finalPayload.entryPoint 
                      << "] but [" 
                      << currentPieces[1] 
                      << "] was requested" << std::endl;
            exit(EXIT_FAILURE);
        }

        if(isLabelInPayload(currentPieces[1]))
        {
            std::cerr << ".init [" << currentPieces[1] << "] already parsed into solace payload." << std::endl;
            exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
        }

        // Ensure name is valid
        if(!isConstNameValid(currentPieces[1]))
        {
            std::cerr << "Invalid name given to constant .int : " << currentPieces[1] << std::endl;
            exit(EXIT_FAILURE);
        }

        // Ensure the thing is an integer
        if(!isInteger(currentPieces[2]))
        {
            std::cerr << "Invalid type given to constant .int : " << currentPieces[2] << std::endl;
            exit(EXIT_FAILURE);
        }

        // Ensure that we haven't had it defined yet
        if(isConstIntInPayload(currentPieces[1]))
        {
            std::cerr << "Constant .int " << currentPieces[1] << " previously defined with value : " << finalPayload.const_ints[currentPieces[1]] << std::endl;
            exit(EXIT_FAILURE);
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
            return;
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
        exit(EXIT_FAILURE);
    }
}

// -----------------------------------------------
//
// -----------------------------------------------

void instruction_create_label()
{
    std::cout << "Create Label: " << currentLine << std::endl;
}