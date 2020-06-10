#ifndef DEL_ENDECODE_HPP
#define DEL_ENDECODE_HPP

#include <string>
#include "Ast.hpp"
#include "Memory.hpp"
#include "IntermediateTypes.hpp"

/*
    Analyzer needs help encoding complex data types in a way that the intermediate layer can make sense of them
    Since I opted for a post-fix expression thing the way I did, this is required. When a node is found to be a complex 
    object, and not a simple operation, we need to use EnDecode to put help build the expression. 

    Once the expression is built it is scanned by the Intermediate object and turned into instructions for the code generator, 
    so this will play the role of decoding the complex types when its time to generate code generator instructions. 

*/

namespace DEL
{
    class EnDecode
    {
    public:
        EnDecode(Memory & memory_man);
        ~EnDecode();

        std::string encode_identifier(std::string identifier);

        std::string encode_call(Call * function_call);

        INTERMEDIATE::TYPES::Directive decode_directive(std::string encoded_call);

    private:
        Memory & memory_man;

        std::string encode_token(std::string token_id);

        INTERMEDIATE::TYPES::DirectiveAllocation decode_allocation(std::string allocation);
    };
}


#endif