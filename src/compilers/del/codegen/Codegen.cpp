#include "Codegen.hpp"

#include "Codeblock.hpp"
#include "Alloc.hpp"
#include "BlockAggregator.hpp"
#include "LoadStore.hpp"
#include "Operations.hpp"
#include "Primitives.hpp"
#include "ConditionalContext.hpp"
#include "LoopContext.hpp"

#include <iostream>
#include <vector>

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Codegen::Codegen(Errors & err, SymbolTable & symbolTable, Memory & memory) : 
                                                                error_man(err), 
                                                                symbol_table(symbolTable),
                                                                memory_man(memory),
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
        // Ensure we're not building a function
        if(building_function)
        {
            std::cerr << "Developer Error : Codegen asked to indicate_complete while building a function " << std::endl;
            exit(EXIT_FAILURE);
        }

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

        // Flag function building
        building_function = true;

        // Create a new function object
        current_function = new CODE::Function(name, params);

        current_aggregator = current_function;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::end_function()
    {
        // Ensure we're building a function
        if(!building_function)
        {
            std::cerr << "Developer Error : Codegen asked to end function while not building function "
                      << "grammar should have prevented this!!!" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Unflag function building
        building_function = false;

        // Reset label id
        label_id = 0;

        //  Indicate how many bytes the function will require to perform all of its operations
        current_function->add_required_bytes(memory_man.get_currently_allocated_bytes_amnt());

        // Finalize the function build with building_complete() and pass the result to the generator to store the 
        // resulting code
        generator.add_instructions(current_function->building_complete());

        // Delete the function object
        delete current_function;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::begin_conditional(CODEGEN::TYPES::ConditionalInitiation conditional_init)
    {
        aggregators.push(new CODE::ConditionalContext(conditional_init));
        
        // Switch the current aggregator to the conditional context
        current_aggregator = aggregators.top();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::extend_conditional(CODEGEN::TYPES::ConditionalInitiation conditional_init)
    {
        // Extend the current conditional context with the information given
        // Aggregator should not change here

        CODE::ConditionalContext * cc = static_cast<CODE::ConditionalContext*>(aggregators.top());
        
        cc->extend_context(conditional_init);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::end_conditional()
    {
        if(aggregators.empty())
        {
            std::cerr << "Developer Error : Codegen asked to end a conditional, but no conditional detected" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Get the context off of the stack
        CODE::ConditionalContext * conditional = static_cast<CODE::ConditionalContext*>(aggregators.top());

        // Pop
        aggregators.pop();

        // Export the conditional as a block - It will be consumed (and deleted by) current_aggregator
        CODE::Block * exported_block = conditional->export_as_block();

        // If the stack is empty redirect the aggregator to be the current function
        if(aggregators.empty())
        {
            current_aggregator = current_function;
        }
        else
        {
            // Otherwise, we want the next context in the stack
            current_aggregator = aggregators.top();
        }

        // Add contents to aggregator - might be the function, might be another context
        current_aggregator->add_block(exported_block);

        // Delete the conditional
        delete conditional;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::begin_loop(CODEGEN::TYPES::LoopInitiation loop_init)
    {
        aggregators.push(new CODE::LoopContext(loop_init));

        // Switch the current aggregator to the conditional context
        current_aggregator = aggregators.top();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::end_loop()
    {
        if(aggregators.empty())
        {
            std::cerr << "Developer Error : Codegen asked to end a loop, but no loop detected" << std::endl;
            exit(EXIT_FAILURE);
        }

        // Get the context off of the stack
        CODE::LoopContext * loop = static_cast<CODE::LoopContext*>(aggregators.top());

        // Pop
        aggregators.pop();

        // Export the loop as a block - It will be consumed (and deleted by) current_aggregator
        // Pass in the allocations so it can generate code to clean each loop pass
        CODE::Block * exported_block = loop->export_as_block();

        // If the stack is empty redirect the aggregator to be the current function
        if(aggregators.empty())
        {
            current_aggregator = current_function;
        }
        else
        {
            // Otherwise, we want the next context in the stack
            current_aggregator = aggregators.top();
        }

        // Add contents to aggregator - might be the function, might be another context
        current_aggregator->add_block(exported_block);

        // Delete the conditional
        delete loop;
    }
    
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::execute_command(CODEGEN::TYPES::Command command)
    {
        // Ensure we're building a function
        if(!building_function)
        {
            std::cerr << "Developer Error : Codegen asked to execute_command while not building function " << std::endl;
            exit(EXIT_FAILURE);
        }

        /*
            command.id                    -> The name of the thing we are assigning for comments
            command.memory_info           -> Where we need to store the thing
            command.classification -> How to treat the given data (int, char, real)
            command.instructions          -> What to do to the data in RPN form
        */

        // Execute the command from the caller
        //
        for(auto & ins : command.instructions)
        {
            switch(ins->instruction)
            {
                case CODEGEN::TYPES::InstructionSet::ADD:    current_aggregator->add_block(new CODE::Addition(command.classification));        break;                                          
                case CODEGEN::TYPES::InstructionSet::SUB:    current_aggregator->add_block(new CODE::Subtraction(command.classification));     break;
                case CODEGEN::TYPES::InstructionSet::DIV:    current_aggregator->add_block(new CODE::Division(command.classification));        break;
                case CODEGEN::TYPES::InstructionSet::MUL:    current_aggregator->add_block(new CODE::Multiplication(command.classification));  break;

                case CODEGEN::TYPES::InstructionSet::RSH:    current_aggregator->add_block(new CODE::RightShift());   break;
                case CODEGEN::TYPES::InstructionSet::LSH:    current_aggregator->add_block(new CODE::LeftShift());    break;
                case CODEGEN::TYPES::InstructionSet::BW_OR:  current_aggregator->add_block(new CODE::BwOr());         break;
                case CODEGEN::TYPES::InstructionSet::BW_NOT: current_aggregator->add_block(new CODE::BwNot());        break;
                case CODEGEN::TYPES::InstructionSet::BW_XOR: current_aggregator->add_block(new CODE::BwXor());        break;
                case CODEGEN::TYPES::InstructionSet::BW_AND: current_aggregator->add_block(new CODE::BwAnd());        break;

                case CODEGEN::TYPES::InstructionSet::LTE:    current_aggregator->add_block(new CODE::Lte(label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::LT:     current_aggregator->add_block(new CODE::Lt (label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::GTE:    current_aggregator->add_block(new CODE::Gte(label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::GT:     current_aggregator->add_block(new CODE::Gt (label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::EQ:     current_aggregator->add_block(new CODE::Eq (label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::NE:     current_aggregator->add_block(new CODE::Neq(label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::OR:     current_aggregator->add_block(new CODE::Or (label_id++, command.classification)); break;
                case CODEGEN::TYPES::InstructionSet::AND:    current_aggregator->add_block(new CODE::And(label_id++, command.classification)); break;

                case CODEGEN::TYPES::InstructionSet::NEGATE: current_aggregator->add_block(new CODE::Negate(label_id++, command.classification)); break;

                case CODEGEN::TYPES::InstructionSet::CALL:   current_aggregator->add_block(new CODE::Call(static_cast<CODEGEN::TYPES::CallInstruction*>(ins))); break;

                case CODEGEN::TYPES::InstructionSet::LOAD:  current_aggregator->add_block(new CODE::Load(static_cast<CODEGEN::TYPES::AddressValueInstruction*>(ins))); break;

                case CODEGEN::TYPES::InstructionSet::STORE: current_aggregator->add_block(new CODE::Store(command.memory_info.start_pos, command.memory_info.bytes_requested, command.id)); break;
 
                case CODEGEN::TYPES::InstructionSet::MOVE_ADDRESS: current_aggregator->add_block(new CODE::MoveAddress(static_cast<CODEGEN::TYPES::MoveInstruction*>(ins))); break;

                case CODEGEN::TYPES::InstructionSet::USE_RAW: current_aggregator->add_block(new CODE::SetupPrimitive(command.id, static_cast<CODEGEN::TYPES::RawValueInstruction*>(ins))); break;

                case CODEGEN::TYPES::InstructionSet::POW:
                {
                    std::string function_name;
                    generator.include_builtin_math_pow(command.classification, function_name);
                    current_aggregator->add_block(new CODE::BuiltIn("POW", function_name));
                    break;
                }
                case CODEGEN::TYPES::InstructionSet::MOD:
                {
                    std::string function_name;
                    generator.include_builtin_math_mod(command.classification, function_name);
                    current_aggregator->add_block(new CODE::BuiltIn("MOD", function_name));
                    break;
                }
                case CODEGEN::TYPES::InstructionSet::RETURN:
                {
                    current_function->build_return();
                    break;
                }
                case CODEGEN::TYPES::InstructionSet::DS_ALLOC:
                {
                    current_aggregator->add_block(new CODE::DSAllocate(static_cast<CODEGEN::TYPES::DSAllocInstruction*>(ins), command.memory_info.start_pos));

                    current_aggregator->add_memory_alloc(command.memory_info);
                    break;
                }
                default:
                    error_man.report_custom("Codegen", "Developer error : Default accessed in command.", true);
                    break;
            }

            /*
                Instruction pointers are deleted by intermediate layer once this function returns
            */
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Codegen::null_return()
    {
        // Ensure we're building a function
        if(!building_function)
        {
            std::cerr << "Developer Error : Codegen asked to make a null_return while not building function " << std::endl;
            exit(EXIT_FAILURE);
        }

        // Tell the function to return without getting information from the stack
        current_function->build_return(false);
    }
}