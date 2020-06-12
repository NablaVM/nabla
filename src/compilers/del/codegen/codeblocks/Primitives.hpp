#ifndef DEL_PRIMITIVE_BLOCKS_HPP
#define DEL_PRIMITIVE_BLOCKS_HPP

#include "Codeblock.hpp"

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
        SetupPrimitive(CODEGEN::TYPES::DataClassification classification, std::string id, std::string value) : Block()
        {
            std::stringstream ss;
            switch(classification)
            {
                /*
                        Integer
                */
                case CODEGEN::TYPES::DataClassification::INTEGER: 
                {
                    // Get the numerical value
                    int64_t i_value = std::stoll(value);

                    // Conditionally little-endian the thing and convert it to an unsigned value
                    uint64_t unsigned_value = ENDIAN::conditional_to_le_64(static_cast<uint64_t>(i_value));

                    code.push_back(std::string(NLT) + "; <<< SETUP INT >>> " + std::string(NL));

                    // Generate the store for 64 bit
                    std::vector<std::string> store_ins = load_64_into_r0(unsigned_value, id);
                    code.insert(code.end(), store_ins.begin(), store_ins.end());
                    break;
                }
                /*
                        Double
                */
                case CODEGEN::TYPES::DataClassification::DOUBLE: 
                {
                    code.push_back(std::string(NLT) + "; <<< SETUP REAL >>> " + std::string(NL));

                    uint64_t unsigned_value = ENDIAN::conditional_to_le_64(
                                                UTIL::convert_double_to_uint64(std::stod(value)
                                                )
                                            );
                    
                    // Generate the store for 64 bit
                    std::vector<std::string> store_ins = load_64_into_r0(unsigned_value, id);
                    code.insert(code.end(), store_ins.begin(), store_ins.end());                       
                    break;
                }
                /*
                        Char
                */
                case CODEGEN::TYPES::DataClassification::CHAR: 
                {
                    code.push_back(std::string(NLT) + "; <<< SETUP CHAR >>> " + std::string(NL));

                    char c_val = value[1];
                    uint32_t u_val = static_cast<uint32_t>(c_val);

                    ss << NLT << "mov" << WS << "r" << REG_ADDR_RO << WS << "$" << std::to_string(u_val) << TAB << "; Move char into reg" << NL;
                    break;
                }
                default:
                    std::cerr << "Error : CodeBlock::SetupValue : Default case reached" << std::endl;
                    exit(EXIT_FAILURE);
                    break;
            }

            ss << NLT << "; ---- Move the value ----" << NLT
                << "pushw" << WS << CALC_STACK << WS << "r" << REG_ADDR_RO << TAB << "; Place on calc stack" << NL;

            code.push_back(ss.str()); 
        }
    };

}
}


#endif