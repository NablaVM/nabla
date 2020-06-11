#include "Codegen.hpp"

#include <iostream>
#include <vector>
#include <limits>
#include <libnabla/util.hpp>
#include <libnabla/endian.hpp>

#include "CodeBlock.hpp"

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Codegen::Codegen(Errors & err, SymbolTable & symbolTable) : 
                                                                error_man(err), 
                                                                symbol_table(symbolTable),
                                                                building_function(false),
                                                                label_id(0)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Codegen::~Codegen()
    {
        
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::vector<std::string> Codegen::indicate_complete()
    {
        // Lock the symbol table so if an error comes out the dev (me) knows they did something dumb
        symbol_table.lock();

        // Create a vector for the result
        std::vector<std::string> result; 
        
        // Indicate to the generator that we are complete
        generator.complete_code_generation(result);
 
        // Return the asm
        return result;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::begin_function(std::string name, std::vector<CODEGEN::TYPES::ParamInfo> params)
    {
        if(building_function)
        {
            std::cerr << "Internal Error >>> Codegen asked to start function while building function "
                      << "grammar should have prevented this!!!" << std::endl;
            exit(EXIT_FAILURE);
        }

        building_function = true;

        current_function = new CODEGEN::Function(name, params);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::end_function()
    {
        if(!building_function)
        {
            std::cerr << "Internal Error >>> Codegen asked to end function while not building function "
                      << "grammar should have prevented this!!!" << std::endl;
            exit(EXIT_FAILURE);
        }

        building_function = false;
        label_id = 0;

        // Finalize the function build with building_complete() and pass the result to the generator to store the 
        // resulting code
        generator.add_instructions(current_function->building_complete());

        delete current_function;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::execute_command(CODEGEN::TYPES::Command command)
    {
        /*
            command.id                    -> The name of the thing we are assigning for comments
            command.memory_info           -> Where we need to store the thing
            command.classification -> How to treat the given data (int, char, real)
            command.instructions          -> What to do to the data in RPN form
        */

        // Add the bytes for the function's stack frame
        //
        current_function->add_required_bytes(command.memory_info.bytes_alloced);

        // Execute the command from the caller
        //
        for(auto & ins : command.instructions)
        {
            switch(ins->instruction)
            {
                case CODEGEN::TYPES::InstructionSet::ADD:    current_function->add_block(new CODE::Addition(command.classification));        break;                                          
                case CODEGEN::TYPES::InstructionSet::SUB:    current_function->add_block(new CODE::Subtraction(command.classification));     break;
                case CODEGEN::TYPES::InstructionSet::DIV:    current_function->add_block(new CODE::Division(command.classification));        break;
                case CODEGEN::TYPES::InstructionSet::MUL:    current_function->add_block(new CODE::Multiplication(command.classification));  break;

                case CODEGEN::TYPES::InstructionSet::RSH:    current_function->add_block(new CODE::RightShift());   break;
                case CODEGEN::TYPES::InstructionSet::LSH:    current_function->add_block(new CODE::LeftShift());    break;
                case CODEGEN::TYPES::InstructionSet::BW_OR:  current_function->add_block(new CODE::BwOr());         break;
                case CODEGEN::TYPES::InstructionSet::BW_NOT: current_function->add_block(new CODE::BwNot());        break;
                case CODEGEN::TYPES::InstructionSet::BW_XOR: current_function->add_block(new CODE::BwXor());        break;
                case CODEGEN::TYPES::InstructionSet::BW_AND: current_function->add_block(new CODE::BwAnd());        break;

                case CODEGEN::TYPES::InstructionSet::LTE:    current_function->add_block(new CODE::Lte(label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::LT:     current_function->add_block(new CODE::Lt (label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::GTE:    current_function->add_block(new CODE::Gte(label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::GT:     current_function->add_block(new CODE::Gt (label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::EQ:     current_function->add_block(new CODE::Eq (label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::NE:     current_function->add_block(new CODE::Neq(label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::OR:     current_function->add_block(new CODE::Or (label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::AND:    current_function->add_block(new CODE::And(label_id++, command.classification)); break;

                case CODEGEN::TYPES::InstructionSet::NEGATE: current_function->add_block(new CODE::Negate(label_id++, command.classification)); break;

                case CODEGEN::TYPES::InstructionSet::CALL:   current_function->add_block(new CODE::Call(static_cast<CODEGEN::TYPES::CallInstruction*>(ins))); break;

                case CODEGEN::TYPES::InstructionSet::LOAD_BYTE:  current_function->add_block(new CODE::Load(CODE::SizeClassification::BYTE, static_cast<CODEGEN::TYPES::AddressValueInstruction*>(ins))); break;
                case CODEGEN::TYPES::InstructionSet::LOAD_WORD:  current_function->add_block(new CODE::Load(CODE::SizeClassification::WORD, static_cast<CODEGEN::TYPES::AddressValueInstruction*>(ins))); break;

                case CODEGEN::TYPES::InstructionSet::STORE_BYTE: current_function->add_block(new CODE::Store(CODE::SizeClassification::BYTE, command.memory_info.start_pos, command.id)); break;
                case CODEGEN::TYPES::InstructionSet::STORE_WORD: current_function->add_block(new CODE::Store(CODE::SizeClassification::WORD, command.memory_info.start_pos, command.id)); break;
 
                case CODEGEN::TYPES::InstructionSet::MOVE_ADDRESS: current_function->add_block(new CODE::MoveAddress(static_cast<CODEGEN::TYPES::MoveInstruction*>(ins))); break;

                 
                case CODEGEN::TYPES::InstructionSet::USE_RAW:
                {
                    CODEGEN::TYPES::RawValueInstruction * rvins = static_cast<CODEGEN::TYPES::RawValueInstruction*>(ins);
                    current_function->add_block(new CODE::SetupPrimitive(command.classification, command.id, rvins->value));
                    break;
                }
                case CODEGEN::TYPES::InstructionSet::POW:
                {
                    std::string function_name;
                    generator.include_builtin_math_pow(command.classification, function_name);
                    current_function->add_block(new CODE::BuiltIn("POW", function_name));
                    break;
                }
                case CODEGEN::TYPES::InstructionSet::MOD:
                {
                    std::string function_name;
                    generator.include_builtin_math_mod(command.classification, function_name);
                    current_function->add_block(new CODE::BuiltIn("MOD", function_name));
                    break;
                }
                case CODEGEN::TYPES::InstructionSet::RETURN:
                {
                    current_function->build_return();
                    break;
                }
                default:
                    error_man.report_custom("Codegen", "Developer error : Default accessed in command.", true);
                    break;
           }
       }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::null_return()
    {
        // Tell the function to return without getting information from the stack
        current_function->build_return(false);
    }



}