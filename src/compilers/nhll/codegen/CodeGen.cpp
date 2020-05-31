#include "CodeGen.hpp"
#include "nhll_postfix.hpp"
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <iostream>

namespace NHLL
{
    // --------------------------------------------
    //
    // --------------------------------------------

    CodeGen::CodeGen() : current_function(nullptr), global_stack_index(0)
    {
        state_stack.push(GenState::IDLE);
    }

    // --------------------------------------------
    //
    // --------------------------------------------
    
    CodeGen::~CodeGen()
    {

    }

    // --------------------------------------------
    //
    // --------------------------------------------

    bool CodeGen::asm_block(std::vector<std::string> asm_code)
    {
        std::cout << ">> CodeGen::asm_block " << std::endl;

        /*
            If the user is bold enough to put asm in their code we will (for now) trust that they know what
            they are doing and just copy the asm into the output. Fate will decide if things work correctly
        */

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
        // as the this scope is nolonger accessable to the use

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
        // as the this scope is nolonger accessable to the use

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

        Variable check;
        if(check_variable_access(name, check) != VariablePollResult::NOT_FOUND)
        {
            std::cerr << "CodeGen::Error : Variable \"" << name << "\" already defined" << std::endl;
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
            return true;
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
        std::cout << ">> CodegEn::global_variable " << std::endl;

        if(state_stack.top() != GenState::IDLE)
        {
            std::cerr << "CodeGen::Error : Must define global variables outside of functions " << std::endl;
            return false;
        }

        /*
            Check all defined variables (functional and global constants) and ensure that this 
            new variable is unique. 

            If this is an expression (i.e not a string) we need to ensure any variable on the RHS
            is also a global. If it isn't its an error. If it is a variable, and it is a global constant
            then we will pre-compute it here and store it in the global stack
        */

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
        std::cout << ">> CodegEn::exit_statement " << std::endl;

        /*
                Just drop in an exit command here. Should be simple
        */
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

    CodeGen::VariablePollResult CodeGen::check_variable_access(std::string name, Variable & definition)
    {
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

                definition = (*rit)[name];
                return VariablePollResult::OKAY;
            }
        }

        // Check constants
        for(auto & v : constants)
        {
            if(v.name == name)
            {
                definition = v;
                return VariablePollResult::OKAY;
            }
        }

        return VariablePollResult::NOT_FOUND;
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
}