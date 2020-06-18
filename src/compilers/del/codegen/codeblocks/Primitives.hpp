#ifndef DEL_PRIMITIVE_BLOCKS_HPP
#define DEL_PRIMITIVE_BLOCKS_HPP

#include "Codeblock.hpp"
#include "SystemSettings.hpp"

#include <iostream>

namespace DEL
{
namespace CODE
{
    //
    //  SetupPrimitive 
    //
    class SetupPrimitive : public Block
    {
    public:
        SetupPrimitive(std::string id, CODEGEN::TYPES::RawValueInstruction* ins) : Block()
        {
            //  Eventually I will do something that will trigger this, and I need to stop it so I don't get a headache when things dont work
            //
            if(SETTINGS::SYSTEM_WORD_SIZE_BYTES != ins->byte_len)
            {
                std::cerr << "Developer Error : SetupPrimitive::SetupPrimitive() received a primitive that was not WORD sized" << std::endl;
                exit(EXIT_FAILURE);
            }

            std::stringstream ss;

            code.push_back(std::string(NL) + std::string(NLT) + "; <<< SETUP PRIMITIVE VARIABLE >>> " + std::string(NL));

            // Build the value into r0
            std::vector<std::string> store_ins = load_64_into_r0(ins->value, id);
            code.insert(code.end(), store_ins.begin(), store_ins.end());     

            ss << NLT << "; ---- Move the value ----" << NLT
                << "pushw" << WS << CALC_STACK << WS << "r" << REG_ADDR_RO << TAB << "; Place on calc stack" << NL;

            code.push_back(ss.str()); 
        }
    };
}
}


#endif