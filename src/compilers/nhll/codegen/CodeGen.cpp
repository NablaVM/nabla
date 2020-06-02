#include "CodeGen.hpp"
#include "CodeGenHelpers.hpp"
#include "nhll_postfix.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>
#include <fstream>
#include <libnabla/assembler.hpp>

namespace NHLL
{
    namespace
    {
        constexpr uint64_t RESERVED_GS_BYTES = 32;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    CodeGen::CodeGen() : current_function(nullptr), global_stack_index(0)
    {
        // Reserve 32 bytes of global stack for system info
        AddressManager::AddressResult adr = addr_mgr.reserve_system_space(RESERVED_GS_BYTES);

        if(adr.num_bits_assigned != RESERVED_GS_BYTES * 8)
        {
            std::cerr << "CodeGen::Error: Unable to reserve GS space for system operations" << std::endl;
            exit(EXIT_FAILURE);
        }   

        state_stack.push(GenState::IDLE);
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    CodeGen::~CodeGen()
    {

    }

    bool CodeGen::finalize()
    {
        /*
            Here we need to finalize any code gen that is left over, and pipe
            the input into the assembler. 

            Once we have the bytes we need to write the functionality in to pass back all of the bytes
            to the CompilerFramework so it can do what it pleases with them
        
        */

        std::vector<std::string> end_result;

        end_result.push_back(".file\t\"App Build with NHLL V.0\"\n");
        end_result.push_back(".init\t__NHLL_INIT_METHOD__\n");

        end_result.push_back("; Bytes reserved for system space: [ 0, " + 
                                std::to_string(RESERVED_GS_BYTES-1) + " ]\n");

        /*
            Build out the constants
        */
        uint64_t int_counter = 0;
        uint64_t str_counter = 0;
        uint64_t dbl_counter = 0;
        for(auto & c : constants)
        {
            switch(c->underlying_prims)
            {
            case UnderlyingPrimitives::INT: 
                end_result.push_back(
                    ".int64\tconstant_integer_"    +  
                    std::to_string(int_counter++) +
                    "\t" + c->definition + "\t; GS: " + 
                    std::to_string(c->address) + "\n");
                break;
            case UnderlyingPrimitives::REAL: 
                end_result.push_back(
                    ".double\tconstant_double_"    +  
                    std::to_string(dbl_counter++) +
                    "\t" + c->definition+ "\t; GS: " + 
                    std::to_string(c->address) + "\n");
                break;
            case UnderlyingPrimitives::STR: 
                end_result.push_back(
                    ".string\tconstant_string_"    +  
                    std::to_string(str_counter++) +
                    "\t" + c->definition + "\t; GS: " +
                    std::to_string(c->address) + "\n");
                break;
            case UnderlyingPrimitives::MIXED: 
                // There won't be any mixed constants here yet, if they ever do show up, 
                // They will be stuffed in the global stack 
            default:
                break;
            }
        }

        //  Pile on all of the functions that have been generated
        //
        for(auto & func : functions)
        {
            end_result.insert(end_result.end(), func.asm_code.begin(), func.asm_code.end());
        }

        std::cout << "CodeGen::finalize()" << std::endl;

        std::ofstream result_out(".nhll_build/asm.s");

        if(!result_out.good())
        {
            std::cerr << "CodeGen::Error : Unable to open build directory for finalization" << std::endl;
            return false;
        }

        for(auto & res : end_result)
        {
            result_out << res;
            std::cout << res;
        }
        result_out.close();

        std::vector<uint8_t> program_data;

        if(!ASSEMBLER::ParseAsm(".nhll_build/asm.s", program_data, true))
        {
            std::cerr << "CodeGen::Error : Assembler failed to assemble final project source" << std::endl;
            return false;
        }

        std::cout << "SUCCESS! Time to pipe the bytes!" << std::endl;

        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::asm_block(std::vector<std::string> asm_code)
    {
        std::cout << ">> CodeGen::asm_block " << std::endl;

        if(current_function == nullptr)
        {
            std::cerr << "CodeGen::Error: current_function was a nullptr when attempting to construct an asm block" << std::endl;
            return false;
        }

        // Indicate that the next section is all on the user
        current_function->asm_code.push_back("; -------- <USER DEFINED ASM BLOCK> --------\n");

        // Put the user asm in
        current_function->asm_code.insert(current_function->asm_code.end(), asm_code.begin(), asm_code.end());

        // Indicate that the user code section is over
        current_function->asm_code.push_back("\n; -------- </USER DEFINED ASM BLOCK> --------\n");

        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::start_function(std::string name, std::vector<FunctionParam> params,  DataPrims return_type)
    {
        std::cout << ">> CodegEn::start_function " << std::endl;

        // If this happens, its on us. Either the preprocessor got bonked, or a grammar rule is broken
        if(state_stack.top() != GenState::IDLE)
        {
            std::cerr << "CodeGen::Error : Can not build function while CodeGen in state other than IDLE" 
                      <<  ". Current state : " << state_to_string(state_stack.top()) << std::endl;
            return false;
        }

        for(auto & f : functions)
        {
            if(f.name == name)
            {
                std::cerr << "CodeGen::Error : Duplicate function \"" << name << "\" discovered" << std::endl;
                return false;
            }
        }

        functions.push_back(
            FunctionRepresentation(name, params, return_type)
        );

        current_function = &functions.back();

        current_function->scoped_variable_map.reserve(100);

        // Replace '.' with '_' for asm function name
        std::replace(name.begin(), name.end(), '.', '_');

        current_function->asm_code.push_back(
        "<" + name + ":\n"
        );

        state_stack.push(GenState::BUILD_FUNCTION);
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::end_function()
    {
        std::cout << ">> CodegEn::end_function " << std::endl;

        // If this happens, its on us. nhll_driver messed up
        if(state_stack.top() != GenState::BUILD_FUNCTION)
        {
            std::cerr << "CodeGen::Error : Finalize function called while CodeGen state was " << state_to_string(state_stack.top()) << std::endl;
            return false;
        }

        // Function end marker
        current_function->asm_code.push_back( ">\n" );

        current_function = nullptr;
        /*
            Do any final checks that may need to occur
        */
        state_stack.pop();
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::start_while(std::string conditional)
    {
        std::cout << ">> CodegEn::start_while " << std::endl;

        std::cout << "\t>>> EXPR (May look odd) >>> " << conditional << std::endl;

        Postfix pf;
        std::vector<Postfix::Element> pf_el = pf.convert(conditional);

        std::cout << std::endl << "\t\t";
        for(auto & i : pf_el )
        {
            std::cout << i.value << " ";
        }
        std::cout << std::endl;


        // Add a new item to the current_function's scoped_variable map to 
        // ensure that new definitions are stored there

        state_stack.push(GenState::BUILD_WHILE);

        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::end_while()
    {
        std::cout << ">> CodegEn::end_while " << std::endl;

        // Need to remove the last element off of the current_function's scoped variable map
        // as the this scope is nolonger accessable to the use - also need to 'delete' the elements in that map

        state_stack.pop();
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::start_loop(std::string name)
    {
        std::cout << ">> CodegEn::start_loop " << std::endl;

        // Add a new item to the current_function's scoped_variable map to 
        // ensure that new definitions are stored there

        state_stack.push(GenState::BUILD_LOOP);

        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::end_loop()
    {
        std::cout << ">> CodegEn::end_loop " << std::endl;

        // Need to remove the last element off of the current_function's scoped variable map
        // as the this scope is nolonger accessable to the use - also need to 'delete' the elements in that map

        state_stack.pop();
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool  CodeGen::start_check()
    {
        if(state_stack.top() == GenState::IDLE)
        {
            std::cerr << "CodeGen::Error : Can not create check statement outside of functions " << std::endl;
            return false;
        }

        std::cout << ">> CodegEn::start_check " << std::endl;
        
        state_stack.push(GenState::BUILD_CHECK);

        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool  CodeGen::end_check()
    {
        std::cout << ">> CodegEn::end_check " << std::endl;
        state_stack.pop();
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::start_check_condition(std::string conditional)
    {
        std::cout << "\t>> CodegEn::start_check_condition " << std::endl;
        
        std::cout << "\t>>> EXPR (May look odd) >>> " << conditional << std::endl;

        Postfix pf;
        std::vector<Postfix::Element> pf_el = pf.convert(conditional);

        std::cout << std::endl << "\t\t";
        for(auto & i : pf_el )
        {
            std::cout << i.value << " ";
        }
        std::cout << std::endl;

        state_stack.push(GenState::BUILD_CHECK_COND);
        return true;
    }
    
    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::end_check_condition()
    {
        std::cout << "\t>> CodegEn::end_check_condition " << std::endl;
        state_stack.pop();
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::declare_variable(std::string name, std::string set_to, bool is_expr)
    {
        if(state_stack.top() == GenState::IDLE)
        {
            std::cerr << "CodeGen::Error : Can not declare variables outside of functions " << std::endl;
            return false;
        }

        std::cout << ">> CodegEn::declare_variable " << std::endl;
        // Make sure it doesn't exist in the current scope


        if(check_global_variable_access(name) != nullptr)
        {
            std::cerr << "CodeGen::Error : Variable \"" << name << "\" already defined as a global" << std::endl;
            return false; 
        }
        if(check_local_variable_access(name) != nullptr)
        {
            std::cerr << "CodeGen::Error : Variable \"" << name << "\" already defined locally" << std::endl;
            return false; 
        }

        /*
            TODO: Setting strings needs to occur after we get some framework setup with the HLL
                  We will need to write some memory management stuff in the ASM code to deal with things
                  that change size. So, for now, we just let out a little message saying that we ignored
                  their command
        */
        if(!is_expr)
        {
            std::cout << ">>>>>>>> Set \"" << name << "\" to string \"" << set_to << "\"" << std::endl;
            std::cout << "Strings are not yet supported. Memory management needs to be completed first" << std::endl;
            return false;
        }

        /*
            TODO: Thanks to the post-fixer we now have a a means to easily compute whatever the expression the user
                  decided to throw at us. The actual computing will be like this : 

                    4 A + 
                
                  Load 4 into stack, Load A into stack, perorm + on last to items, removing them from the stack. Place result
                  in the stack. BOOM computed. The whole expression can be done this way. Postfixer returns a vector
                  that tells us if something is an operation or en element which is nice. Now all we need to do is figure out
                  if the element is avariable or not and decide what type (if any) we need to up-convert to.
        */
        std::cout << ">>>>>>>> Set \"" << name << "\" to expression \"" << set_to << "\"" << std::endl;

        // if its not an expression its a string
        if(is_expr)
        {
            Postfix pf;
            std::vector<Postfix::Element> pf_el = pf.convert(set_to);

            std::cout << std::endl << "\t\t";
            for(auto & i : pf_el )
            {
                std::cout << i.value << " ";
            }
            std::cout << std::endl;

        }
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    bool CodeGen::declare_integer(std::string name, std::string expression)
    {
        std::cout << ">> CodeGen : Declare Int : " << name << " : " << expression << std::endl;

        // Ensure it doesn't exist, we aren't in the idle state, and that current_function is valid
        if(!allowed_to_set_variable(name))
        {
            return false;
        }
        
        // Integers are defaulted to 64-bit signed integers (for now)
        AddressManager::AddressResult addr_res = addr_mgr.new_user_stack_variable(8);

        // Vector to hold resulting assembly code
        std::vector<std::string> code_vector;
        
        // Construct the expression code
        if(!construct_expression(name, expression, addr_res.address, code_vector))
        {
            return false;
        }

        // Add the variable to the current scope map if the expression was a success

        auto scope_map = current_function->scoped_variable_map[current_function->scoped_variable_map.size()];
        scope_map[name] = new Nhll_Integer(addr_res.address, name, expression);

        // Add the asm code to the function's asm code
        current_function->asm_code.insert(current_function->asm_code.end(), code_vector.begin(), code_vector.end());

        // Indicate complete
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::declare_real(std::string name, std::string expression)
    {
        std::cout << ">> CodeGen : Declare Real : " << name << " : " << expression << std::endl;
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::declare_string(std::string name, std::string value, uint64_t allowed_size)
    {
        std::cout << ">> CodeGen : Declare String : " << name << " : " << value << " | " << allowed_size << std::endl;
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::reassign_variable(std::string name, std::string set_to, bool is_expr)
    {
        if(state_stack.top() == GenState::IDLE)
        {
            std::cerr << "CodeGen::Error : Can not declare variables outside of functions " << std::endl;
            return false;
        }

        std::cout << ">> CodegEn::reassign_variable " << std::endl;

        // Make sure it exists in a scope we can reach it (see above)

        // if its not an expression its a string

        // if it is an expression need to analyze to see if variables exist that need to be reolved, 
        // and need to figure out if the expression will become a double or int

        // Ensure that the destination type matches the type that set_to will be evaluated to

        std::cout << ">>>>>>>> Set \"" << name << "\" to \"" << set_to << "\"" << std::endl;

        if(is_expr)
        {
            Postfix postfixer;
            
            auto el_list = postfixer.convert(set_to);
        }
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::global_variable(std::string name, std::string set_to, bool is_expr)
    {
        // Check in-scope locals (shouldnt be any)
        BaseVariable * local_search = check_local_variable_access(name);

        if(local_search != nullptr)
        {
            std::cerr << "CodeGen::Error : Global variable \"" << name 
                      << "\" previously defined as local variable \"" << local_search->definition << std::endl;
            return false;
        }
    

        // Check other global definitions
        BaseVariable * global_search = check_local_variable_access(name);

        if(global_search != nullptr)
        {
            std::cerr << "CodeGen::Error : Global variable \"" << name 
                      << "\" previously defined as \"" << global_search->definition << std::endl;
            return false;
        }


        // Analyze expression
        if(is_expr)
        {
            Postfix postfixer;
            auto el_list = postfixer.convert(set_to);

            bool pre_compute_as_double = false;

            //  Check for single value expressions (raw values)
            //
            if(el_list.size() == 1)
            {
                if(el_list[0].type == Postfix::Type::VALUE)
                {
                    if(HELPERS::is_number<int>(el_list[0].value))
                    {
                        try
                        {
                            std::stol(el_list[0].value);
                        }
                        catch(...)
                        {
                            std::cerr << "CodeGen::Error : Given number \"" <<  el_list[0].value << "\" exceeds currently supported size (signed 64 bit integer)" << std::endl;
                            return false; 
                        }

                        AddressManager::AddressResult addr_res = addr_mgr.new_global_integer();
                        constants.push_back(new Nhll_Integer(addr_res.address, name, set_to));
                    }
                    else
                    {
                        AddressManager::AddressResult addr_res = addr_mgr.new_global_double();
                        constants.push_back(new Nhll_Real(addr_res.address, name, set_to));
                    }
                    return true;
                }
                else
                {
                    std::cerr << "CodeGen::Error : Somehow a single op was identified as an expression." 
                              << " If you're seeing this then the grammar is broken."
                              << "\nOp identified : " << el_list[0].value << std::endl;
                    return false;
                }

                std::cerr << "CodeGen::Error : Single item expression not matched at all. Thid shouldn't have happened" << std::endl;
                return false;
            }

            // Check expression for variables
            for(auto & element : el_list)
            {
                if(element.type == Postfix::Type::VALUE)
                {
                    if(HELPERS::is_number<int>(element.value))
                    {
                        // Do nothing we assume its an int
                    }
                    else if (HELPERS::is_number<double>(element.value))
                    {
                        pre_compute_as_double = true;
                    }
                    else
                    {
                        // Make sure the variable exists as a global
                        BaseVariable * glob_search = check_global_variable_access(element.value);
                        if(glob_search == nullptr)
                        {
                            std::cerr << "CodeGen::Error : Global variable \"" << element.value << "\" in definition of \"" << name << "\" has not been defined " << std::endl; 
                            return false;
                        }

                        element.value = glob_search->definition;
                    }
                }
            }

            // Precompute the global's value - Element values have been converted from their varible name to their value
            std::string pre_computed_value;

            // Compute expression as doubles
            if(pre_compute_as_double)
            {
                pre_computed_value = HELPERS::pre_compute<double> (el_list);

                AddressManager::AddressResult addr_res = addr_mgr.new_global_double();
                constants.push_back(new Nhll_Real(addr_res.address, name, pre_computed_value));
            }
            // Compute expression as integers
            else
            {
                pre_computed_value = HELPERS::pre_compute<int64_t> (el_list);

                try
                {
                    std::stol(pre_computed_value);
                }
                catch(...)
                {
                    std::cerr << "CodeGen::Error : Pre-Computed Global \"" <<  pre_computed_value << "\" exceeds currently supported size (signed 64 bit integer)" << std::endl;
                    return false; 
                }
                
                AddressManager::AddressResult addr_res = addr_mgr.new_global_integer();
                constants.push_back(new Nhll_Integer(addr_res.address, name, pre_computed_value));
            }
            return true;
        } // End of expression computation

        // If we get here its a string
        AddressManager::AddressResult addr_res = addr_mgr.new_global_string(set_to.size());
        constants.push_back(new Nhll_String(addr_res.address, name, set_to));
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::break_loop(std::string name)
    {
        std::cout << ">> CodegEn::break_loop " << std::endl;

        /*
            Need to come up with a method for matching the given name to a loop that we assume we are in.
            If we are not in a loop with a given name we need to die and yell at the user.
        */

        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::call_method(std::string method, std::vector<std::string> params)
    {
        std::cout << ">> CodegEn::call_method " << std::endl;

        /*
            Need to iterate over functions and see if they exist yet. If they don't we need to store this call into a table
            with information regarding where the hell we are in the code so we can resolve the call later if the function eventually
            decides to show up.

            We will also need to check that the given parameters conform to the function signature that this call is attempting to go to.
            Since we just have them as strings they could literally be anything, so a little bit of regex will have to happen and then
            a comparison to the callee signature. 

            If the thing doesn't exist yet then we'll do that resolve whenever it shows up. Maybe a hook can be put into
            start_function that checks if a pending call link exists and if so the check can be done then, with any errors pointed at 
            the actual call line.
        */

        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::yield_statement(std::string value, bool is_expr)
    {
        std::cout << ">> CodegEn::yield_statement " << std::endl;

        /*
            Need to check that the value (if there is one) matches the type that is expected
            from the current function
        */
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::return_statement(std::string value, bool is_expr)
    {
        std::cout << ">> CodegEn::return_statement " << std::endl;

        /*
            Need to check that the value (if there is one) matches the type that is expected
            from the current function
        */
        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::exit_statement()
    {
        if(current_function == nullptr)
        {
            std::cerr << ">> CodeGen::Error : current_function was a nullptr when attempting to construct an exit" << std::endl;
        }

        current_function->asm_code.push_back(
        "\texit\n"
        );
        return true;
    }

    // ------------------- Ugly comment block so I can easily see where private meets public in the viewer -----
    // ----------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------

    // --------------------------------------------
    //  Given a variable name attempt to get its information
    //  so the caller can check type/ access/ etc
    // --------------------------------------------

    BaseVariable * CodeGen::check_local_variable_access(std::string name)
    {
        if(!current_function)
        {
            return nullptr;
        }

        // Go through current function's scope variable map and see if 
        // the variable given exists, and hand back the Variable definition

        // Get a reverse iterator that will go over all listed variable maps
        auto rit = current_function->scoped_variable_map.rbegin();

        // Check each variable map for a key that identifies the variable in question
        for (; rit!= current_function->scoped_variable_map.rend(); ++rit)
        {
            if ((*rit).find(name) != (*rit).end() )
            {
                // Variable found
                return  (*rit)[name];
            }
        }
        return nullptr;
    }

    // --------------------------------------------
    //
    // --------------------------------------------

    BaseVariable * CodeGen::check_global_variable_access(std::string name)
    {

        for(uint64_t i = 0; i < constants.size(); i++)
        {
            if(constants[i] != nullptr)
            {
                if(constants[i]->name == name)
                {
                    return constants[i];
                }
            }
        }

        return nullptr;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    std::string CodeGen::state_to_string(CodeGen::GenState state) const
    {
        switch(state)
        {
            case CodeGen::GenState::IDLE:             return "IDLE";
            case CodeGen::GenState::BUILD_FUNCTION:   return "BUILD_FUNCTION";
            case CodeGen::GenState::BUILD_WHILE:      return "BUILD_WHILE";
            case CodeGen::GenState::BUILD_LOOP:       return "BUILD_LOOP";
            case CodeGen::GenState::BUILD_CHECK:      return "BUILD_CHECK";
            case CodeGen::GenState::BUILD_CHECK_COND: return "BUILD_CHECK_COND";
            default:
                std::cerr << "CodeGen::Error : Trying to print an invalid state!" << std::endl;
                exit(EXIT_FAILURE);
                break;
        }
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    bool CodeGen::allowed_to_set_variable(std::string name)
    {
        if(state_stack.top() == GenState::IDLE)
        {
            std::cerr << "CodeGen::Error : Can not declare variables outside of functions " << std::endl;
            return false;
        }

        if(check_global_variable_access(name) != nullptr)
        {
            std::cerr << "CodeGen::Error : Variable \"" << name << "\" already defined as a global" << std::endl;
            return false; 
        }

        if(check_local_variable_access(name) != nullptr)
        {
            std::cerr << "CodeGen::Error : Variable \"" << name << "\" already defined locally" << std::endl;
            return false; 
        }

        if(current_function == nullptr)
        {
            std::cerr << "CodeGen::Error : Current function was a nullptr when attempting to set a variable" << std::endl;
            return false;
        }

        return true;
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    std::vector<std::string> CodeGen::setup_init_function()
    {
        std::vector<std::string> result;

        result.push_back("<__NHLL_INIT_METHOD__:\n");

        // Expand gs to hold program stack and reserved space

        // Insert program stack start position in GS 0 


        result.push_back(">\n");

        return result;
    }
    
    // --------------------------------------------
    //
    // --------------------------------------------
    
    bool CodeGen::construct_expression(std::string name, std::string expression, uint64_t address_to, std::vector<std::string> &result)
    {
        result.clear();

        std::cout << ">>> CodeGen::Construct Expresison! " << std::endl;

        // Assume int, if a double is present, the whole thing will be promoted to a double.


        return true;
    }

}