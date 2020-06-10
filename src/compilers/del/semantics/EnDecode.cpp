#include "EnDecode.hpp"

#include <vector>
#include <string>
#include <sstream>
#include <iostream>

namespace DEL
{
    namespace
    {
        std::vector<std::string> split(std::string s, char c)
        {
            std::string segment;
            std::vector<std::string> list;
            std::stringstream ss(s);
            while(std::getline(ss, segment, c))
            {
                list.push_back(segment);
            }
            return list;
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    EnDecode::EnDecode(Memory & memory_man) : memory_man(memory_man)
    {

    }
       
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    EnDecode::~EnDecode()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string EnDecode::encode_identifier(std::string identifier)
    {
        Memory::MemAlloc mem_info = memory_man.get_mem_info(identifier);

        return encode_token("ID") + 
                std::to_string(mem_info.start_pos) + 
                "|" + 
                std::to_string(mem_info.bytes_alloced); 
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string EnDecode::encode_call(Call * function_call)
    {
        std::string result = encode_token("CALL:" + function_call->name);

        for(uint64_t i = 0; i < function_call->params.size(); i++)
        {
            Memory::MemAlloc mem_info = memory_man.get_mem_info(function_call->params[i].id);

            result += std::to_string(mem_info.start_pos);
            result += ("|" + std::to_string(mem_info.bytes_alloced));

            if(i != function_call->params.size()-1)
            {
                result += ",";
            }
        }

        return result;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    INTERMEDIATE::TYPES::Directive EnDecode::decode_directive(std::string directive)
    {
        INTERMEDIATE::TYPES::Directive result_directive;

        directive = directive.substr(1, directive.size());

        // Get the directive tokens
        std::vector<std::string> d_list = split(directive, ':');

        int8_t alloc_location = 1;
        if(d_list[0] == "ID")        
        { 
            result_directive.type = INTERMEDIATE::TYPES::DirectiveType::ID;   
        }
        else if(d_list[0] == "CALL") 
        { 
            alloc_location = 2;
            result_directive.data = d_list[1];
            result_directive.type = INTERMEDIATE::TYPES::DirectiveType::CALL; 
        }
        else
        {
            std::cerr << "Developer Error >>> EnDecoder was handed a directive it didn't understand : "
                      << d_list[0] << std::endl;
            exit(EXIT_FAILURE);
        }

        // Get the allocation tokens
        std::vector<std::string> a_list = split(d_list[alloc_location], ',');

        for(auto & i : a_list)
        {
            result_directive.allocation.push_back(
                decode_allocation(i)
            );
        }

        return result_directive;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string EnDecode::encode_token(std::string token_id)
    {
        return "#" + token_id + ":";
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    INTERMEDIATE::TYPES::DirectiveAllocation EnDecode::decode_allocation(std::string allocation)
    {
        std::vector<std::string> alloc_tokens = split(allocation, '|');

        // We but a try here because there is an edge case where the tokens aren't anything and can sometimes cause issues.
        // Instead of checking for them in every possible case, we try catch. If the edge case is present, they expected 0,0
        // anyway, so everyone but me is happy
        try
        {
            uint64_t start_pos = std::stoull(alloc_tokens[0]);
            uint64_t end_pos   = start_pos + std::stoull(alloc_tokens[1]);

            return INTERMEDIATE::TYPES::DirectiveAllocation{ 
                start_pos,
                end_pos
            };
        }
        catch(...)
        {
            return INTERMEDIATE::TYPES::DirectiveAllocation{ 
                0,
                0
            };
        }
    }
}