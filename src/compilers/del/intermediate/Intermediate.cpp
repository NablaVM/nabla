#include "Intermediate.hpp"

#include <sstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <libnabla/endian.hpp>
#include <libnabla/util.hpp>
#include "CodegenTypes.hpp"
#include "EnDecode.hpp"
#include "SystemSettings.hpp"

#define N_UNUSED(x) (void)x;

namespace DEL
{
    namespace
    {
        template<typename Numeric>
        inline static bool is_number(const std::string& s)
        {
            Numeric n;
            return((std::istringstream(s) >> n >> std::ws).eof());
        }

        bool is_only_number(std::string v)
        {
            try
            {
                double value = std::stod(v);
                N_UNUSED(value)
                return true;
            }
            catch(std::exception& e)
            {
                // Its not a number
            }
            return false;
        }

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

    Intermediate::Intermediate(Memory & memory_man, Codegen & code_gen) : 
            memory_man(memory_man), code_gen(code_gen)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Intermediate::~Intermediate()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_start_function(std::string name, std::vector<FunctionParam> params)
    {
        std::vector<CODEGEN::TYPES::ParamInfo> codegen_params;

        // Its important to start at 8. THE GS is byte-wise, we are operating word-wise
        uint16_t gs_param_start = SETTINGS::GS_FRAME_OFFSET_RESERVE * SETTINGS::SYSTEM_WORD_SIZE_BYTES;

        for(auto & p : params)
        {
            Memory::MemAlloc mem_info = memory_man.get_mem_info(p.id);

            codegen_params.push_back(CODEGEN::TYPES::ParamInfo{
                mem_info.start_pos,
                mem_info.start_pos + mem_info.bytes_alloced,
                gs_param_start
            });
            gs_param_start += SETTINGS::SYSTEM_WORD_SIZE_BYTES;
        }

        code_gen.begin_function(name, codegen_params);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_end_function()
    {
        code_gen.end_function();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_null_return()
    {
        code_gen.null_return();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_start_conditional_context(Memory::MemAlloc memory_info )
    {
        code_gen.begin_conditional(CODEGEN::TYPES::ConditionalInitiation(memory_info));
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_trailed_context(Memory::MemAlloc memory_info)
    {
        code_gen.extend_conditional(CODEGEN::TYPES::ConditionalInitiation(memory_info));
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_end_conditional_context()
    {
        code_gen.end_conditional();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_direct_call(std::string encoded_call)
    {
        // Create the command
        CODEGEN::TYPES::Command command;

        // We know its a call directive, so we pass it to build directive
        build_assignment_directive(command, encoded_call, 1);

        // Because the standard case is to expect a return value, we need to edit the command before
        // We send it off
        CODEGEN::TYPES::CallInstruction * c = static_cast<CODEGEN::TYPES::CallInstruction *>(command.instructions.back());

        // We don't expect return values from direct calls
        c->expect_return_value = false;

        // Execute the call command
        code_gen.execute_command(command);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_start_loop(INTERMEDIATE::TYPES::LoopIf * loop)
    {
        switch(loop->type)
        {
            case INTERMEDIATE::TYPES::LoopTypes::FOR:
            {
                // Cast to the loop type
                INTERMEDIATE::TYPES::ForLoop * fl = static_cast<INTERMEDIATE::TYPES::ForLoop*>(loop);

                // Convert type to one codegen understands
                CODEGEN::TYPES::DataClassification dc = (fl->classification == INTERMEDIATE::TYPES::AssignmentClassifier::DOUBLE ) ? 
                                    CODEGEN::TYPES::DataClassification::DOUBLE :
                                    CODEGEN::TYPES::DataClassification::INTEGER;

                // Create codegen loopinit
                CODEGEN::TYPES::LoopInitiation loop_init(dc, fl->var, fl->end, fl->step);

                // Begin the loop
                code_gen.begin_loop(loop_init);
                break;
            }
            case INTERMEDIATE::TYPES::LoopTypes::NAMED:
            {
                std::cerr << "UNHANDLED LOOP TYPE : 'NAMED' IN INTERMEDIATE >> DEVELOPER ERROR" << std::endl;
                exit(EXIT_FAILURE);
                break;
            }
            case INTERMEDIATE::TYPES::LoopTypes::WHILE:
            {
                std::cerr << "UNHANDLED LOOP TYPE : 'WHILE' IN INTERMEDIATE >> DEVELOPER ERROR" << std::endl;
                exit(EXIT_FAILURE);
                break;
            }
            default:
                std::cerr << "DEFAULT accessed in determining loop type in Intermediate::issue_start_loop()" << std::endl;
                exit(EXIT_FAILURE);
                break;
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_end_loop()
    {
        code_gen.end_loop();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::issue_assignment(std::string id, bool requires_ds_allocation, Memory::MemAlloc memory_info, INTERMEDIATE::TYPES::AssignmentClassifier classification, std::string postfix_expression)
    {
        // Build the instruction set
        CODEGEN::TYPES::Command command = encode_postfix_assignment_expression(requires_ds_allocation, memory_info, classification, postfix_expression);
        command.id = id;

        // Issue the command
        code_gen.execute_command(command);

        // Clean out the instructions
        for(auto & i : command.instructions)
        {
            delete i;
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    CODEGEN::TYPES::Command Intermediate::encode_postfix_assignment_expression(bool rdsa, Memory::MemAlloc memory_info,  INTERMEDIATE::TYPES::AssignmentClassifier classification, std::string expression)
    {
        // Build the expression into a string vector
        std::istringstream buf(expression);
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> tokens(beg, end);

        // Build instructions for command
        CODEGEN::TYPES::Command command;

        command = build_assignment(rdsa, classification, tokens, memory_info.bytes_requested);

        // Information regarding where to store result
        command.memory_info = memory_info;

        return command;
    }
    
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    CODEGEN::TYPES::Command Intermediate::build_assignment(bool rdsa, INTERMEDIATE::TYPES::AssignmentClassifier & classification, std::vector<std::string> & tokens, uint64_t byte_len)
    {
        CODEGEN::TYPES::Command command;

        // Indicate how raw values should be interpd
        switch(classification)
        {
        case INTERMEDIATE::TYPES::AssignmentClassifier::CHAR:    command.classification = CODEGEN::TYPES::DataClassification::INTEGER; break;
        case INTERMEDIATE::TYPES::AssignmentClassifier::INTEGER: command.classification = CODEGEN::TYPES::DataClassification::INTEGER; break;
        case INTERMEDIATE::TYPES::AssignmentClassifier::DOUBLE:  command.classification = CODEGEN::TYPES::DataClassification::DOUBLE;  break;
        default: std::cerr << "Devloper Error >>> Intermediate::encode_postfix_assignment_expression() classification switch reached default : " << std::endl;
                 exit(EXIT_FAILURE);
                 break;
        }

        // Check all tokens for what they represent
        for(auto & token : tokens)
        {
            // Check for a directive
            if(token[0] == '#')
            {
                build_assignment_directive(command, token, byte_len);
            }
            // Check for char || int || double (raw values)
            else if(token[0] == '"' || is_only_number(token) )
            {
                command.instructions.push_back(
                    new CODEGEN::TYPES::RawValueInstruction(CODEGEN::TYPES::InstructionSet::USE_RAW, decompose_primitive(classification, token), byte_len)
                );
            }
            else
            {
                // Get the operation token
                command.instructions.push_back(
                    new CODEGEN::TYPES::BaseInstruction(get_operation(token))
                );
            }
        }

        // If its a return statement, we don't want to add a store command
        if(command.instructions.back()->instruction == CODEGEN::TYPES::InstructionSet::RETURN)
        {
            return command;
        }

        // Requires ds allocation
        if(rdsa)
        {
            command.instructions.push_back(
                new CODEGEN::TYPES::DSAllocInstruction(CODEGEN::TYPES::InstructionSet::DS_ALLOC, byte_len)
            );
        }

        // End of assignment trigger storage of result
        command.instructions.push_back(
            new CODEGEN::TYPES::BaseInstruction(CODEGEN::TYPES::InstructionSet::STORE)
        );
        return command;
    }

    // ----------------------------------------------------------
    // 
    // ----------------------------------------------------------

    uint64_t Intermediate::decompose_primitive(INTERMEDIATE::TYPES::AssignmentClassifier & classification, std::string value)
    {
        switch(classification)
        {
            case INTERMEDIATE::TYPES::AssignmentClassifier::CHAR:
            {
                return ENDIAN::conditional_to_le_64(static_cast<uint64_t>(value[1]));
            }
            case INTERMEDIATE::TYPES::AssignmentClassifier::INTEGER:
            {
                return ENDIAN::conditional_to_le_64(std::stoull(value));
            }
            case INTERMEDIATE::TYPES::AssignmentClassifier::DOUBLE:
            {
                return ENDIAN::conditional_to_le_64(UTIL::convert_double_to_uint64(std::stod(value)));
            }
            default: std::cerr << "Devloper Error >>> Intermediate::decompose_primitive() classification switch reached default : " << std::endl;
                exit(EXIT_FAILURE);
                break;
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Intermediate::build_assignment_directive(CODEGEN::TYPES::Command & command, std::string directive_token, uint64_t byte_len)
    {
        EnDecode endecode(memory_man);

        INTERMEDIATE::TYPES::Directive directive = endecode.decode_directive(directive_token);

        // Figure out what type the directive is directing us to do
        switch(directive.type)
        {
            // Handle an ID
            case INTERMEDIATE::TYPES::DirectiveType::ID:
            {
                command.instructions.push_back(
                    new CODEGEN::TYPES::AddressValueInstruction(CODEGEN::TYPES::InstructionSet::LOAD,
                        directive.allocation[0].start_pos, byte_len
                    )
                );
                break;
            }

            // Handle a call
            case INTERMEDIATE::TYPES::DirectiveType::CALL:
            {
                // Go through call and create CODEGEN::TYPES::MoveInstructions to move
                // local variables to the parameter passing zone
                uint64_t param_gs_slot = SETTINGS::GS_FRAME_OFFSET_RESERVE * SETTINGS::SYSTEM_WORD_SIZE_BYTES;
                for(auto & d : directive.allocation)
                {
                    command.instructions.push_back(
                        new CODEGEN::TYPES::MoveInstruction(CODEGEN::TYPES::InstructionSet::MOVE_ADDRESS,
                            param_gs_slot,
                            d.start_pos,
                            d.end_pos - d.start_pos
                        )
                    );
                    param_gs_slot += SETTINGS::SYSTEM_WORD_SIZE_BYTES;
                }

                // Call the function
                command.instructions.push_back(
                    new CODEGEN::TYPES::CallInstruction(CODEGEN::TYPES::InstructionSet::CALL, directive.data)
                );
                break;
            }

            default:
            {
                // Shouldn't ever happen 
                std::cerr << "Developer Error >>> Intermediate was handed a wonky token from the EnDecoder" << std::endl;
                exit(EXIT_FAILURE);
                break; 
            }
        }
    }

    // ----------------------------------------------------------
    // 
    // ----------------------------------------------------------

    CODEGEN::TYPES::InstructionSet Intermediate::get_operation(std::string token)
    {
        if(token == "ADD"   ) { return CODEGEN::TYPES::InstructionSet::ADD;    }
        if(token == "SUB"   ) { return CODEGEN::TYPES::InstructionSet::SUB;    }
        if(token == "MUL"   ) { return CODEGEN::TYPES::InstructionSet::MUL;    }
        if(token == "DIV"   ) { return CODEGEN::TYPES::InstructionSet::DIV;    }
        if(token == "MOD"   ) { return CODEGEN::TYPES::InstructionSet::MOD;    }
        if(token == "POW"   ) { return CODEGEN::TYPES::InstructionSet::POW;    }
        if(token == "LTE"   ) { return CODEGEN::TYPES::InstructionSet::LTE;    }
        if(token == "LT"    ) { return CODEGEN::TYPES::InstructionSet::LT;     }
        if(token == "GTE"   ) { return CODEGEN::TYPES::InstructionSet::GTE;    }
        if(token == "GT"    ) { return CODEGEN::TYPES::InstructionSet::GT;     }
        if(token == "EQ"    ) { return CODEGEN::TYPES::InstructionSet::EQ;     }
        if(token == "NE"    ) { return CODEGEN::TYPES::InstructionSet::NE;     }
        if(token == "LSH"   ) { return CODEGEN::TYPES::InstructionSet::LSH;    }
        if(token == "RSH"   ) { return CODEGEN::TYPES::InstructionSet::RSH;    }
        if(token == "NEGATE") { return CODEGEN::TYPES::InstructionSet::NEGATE; }
        if(token == "OR"    ) { return CODEGEN::TYPES::InstructionSet::OR;     }
        if(token == "AND"   ) { return CODEGEN::TYPES::InstructionSet::AND;    }
        if(token == "BW_OR" ) { return CODEGEN::TYPES::InstructionSet::BW_OR;  }
        if(token == "BW_XOR") { return CODEGEN::TYPES::InstructionSet::BW_XOR; }
        if(token == "BW_AND") { return CODEGEN::TYPES::InstructionSet::BW_AND; }
        if(token == "BW_NOT") { return CODEGEN::TYPES::InstructionSet::BW_NOT; }
        if(token == "RETURN") { return CODEGEN::TYPES::InstructionSet::RETURN; }
        std::cerr << "Developer error : Intermediate::InstructionSet Intermediate::get_integer_operation(std::string token)" << std::endl;
        exit(EXIT_FAILURE);
    }
}