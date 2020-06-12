#include "Analyzer.hpp"

#include <iostream>
#include <regex>
#include <sstream>

#include "SystemSettings.hpp"

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Analyzer::Analyzer(Errors & err, SymbolTable & symbolTable, Codegen & code_gen, Memory & memory) : 
                                                                        error_man(err), 
                                                                        symbol_table(symbolTable),
                                                                        memory_man(memory),
                                                                        endecoder(memory_man),
                                                                        intermediate_layer(memory, code_gen)
    {
        program_watcher.setup();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    Analyzer::~Analyzer()
    {
        
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::check_for_finalization()
    {
        if(!program_watcher.has_main)
        {
            error_man.report_no_main_function();
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::ensure_unique_symbol(std::string id)
    {
        if(symbol_table.does_symbol_exist(id, true))
        {
            error_man.report_previously_declared(id);
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::ensure_id_in_current_context(std::string id, std::vector<ValType> allowed)
    {
        // Check symbol table to see if an id exists, don't display information yet
        if(!symbol_table.does_symbol_exist(id, false))
        {
            // Reports the error and true marks the program for death
            error_man.report_unknown_id(id, true);
        }

        // If allowed is empty, we just wanted to make sure the thing existed
        if(allowed.size() == 0)
        {
            return;
        }

        // Ensure type is one of the allowed types
        bool is_allowed = false;
        for(auto & v : allowed)
        {
            if(symbol_table.is_existing_symbol_of_type(id, v))
            {
            is_allowed = true;
            }
        }

        // If the type isn't allowed
        if(!is_allowed)
        {
            error_man.report_unallowed_type(id, true);
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    ValType Analyzer::get_id_type(std::string id)
    {
        ValType t = symbol_table.get_value_type(id);

        if(t == ValType::NONE)
        {
            error_man.report_unknown_id(id, true);
        }

        return t;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::build_function(Function *function)
    {
        // Ensure function is unique
        if(symbol_table.does_context_exist(function->name))
        {
            // Dies if not unique
            error_man.report_previously_declared(function->name);
        }

        // Check for 'main'
        if(function->name == "main")
        {
            program_watcher.has_main = true;
        }

        // Check for passign the hard-set limit on parameters
        if(function->params.size() > SETTINGS::GS_FUNC_PARAM_RESERVE)
        {
            std::string error = " Given function exceeds current limit of '" + std::to_string(SETTINGS::GS_FUNC_PARAM_RESERVE) + "' parameters";

            error_man.report_custom("Analyzer::build_function", error, true);
        }

        // Create function context
        // Don't remove previous context.. we clear the variables out later
        symbol_table.new_context(function->name, false );

        // Place function parameters into context
        for(auto & p : function->params)
        {
            symbol_table.add_symbol(p.id, p.type);
        }

        // Add parameters to the context
        symbol_table.add_parameters_to_current_context(function->params);

        // Add return type to the context
        symbol_table.add_return_type_to_current_context(function->return_type);

        // Tell intermediate layer to start function with given parametrs
        intermediate_layer.issue_start_function(function->name, function->params);

        // So elements can access function information as we visit them
        current_function = function;

        // Keep an eye out for pieces that we enforce in a function
        function_watcher.has_return = false;

        // Iterate over function elements and visit them with *this
        for(auto & el : function->elements)
        {
            // Visiting elements will trigger analyzer to check particular element
            // for any errors that may be present, and then analyzer will ask Intermediate to
            // generate instructions for the Codegen / Send the instructions to code gen
            el->visit(*this);

            // Now that the item is constructed, we free the memory
            delete el;
        }

        // Tell intermediate layer that we are done constructin the current function
        intermediate_layer.issue_end_function();

        // Clear the symbol table for the given function so elements cant be accessed externally
        // We dont delete the context though, that way can confirm existence later
        symbol_table.clear_existing_context(function->name);

        if(!function_watcher.has_return)
        {
            error_man.report_no_return(function->name);
        }

        current_function = nullptr;

        // Reset the memory manager for alloc variables in new space
        memory_man.reset();

        // Function is constructed - and elements have been freed
        delete function;
    }

    // -----------------------------------------------------------------------------------------
    // 
    //                              Visitor Methods
    //
    // -----------------------------------------------------------------------------------------

    void Analyzer::accept(Assignment &stmt)
    {
        /*
            If the assignment is a reassignment, it will be indicated via REQ_CHECK. 
            If this is active, we need to ensure it exists in reach and ensure that the type 
            allows us to do assignments. 
        */


        Memory::MemAlloc memory_info;

        bool requires_allocation_in_symbol_table = true;
        if(stmt.data_type == ValType::REQ_CHECK)
        {
            // Check that the rhs is in context and is a type that we are allowing for assignment
            // NOTE : When functions are allowed in asssignment this will have to be updated <<<<<<<<<<<<<<<<<<
            ensure_id_in_current_context(stmt.lhs, {ValType::INTEGER, ValType::REAL, ValType::CHAR});

            // Now we know it exists, we set the data type to what it states in the map
            stmt.data_type = get_id_type(stmt.lhs);

            // We already checked symbol table if this exists, and symbol table is what 
            // handles allocation of memory for the target, so this is safe
            memory_info = memory_man.get_mem_info(stmt.lhs);

            requires_allocation_in_symbol_table = false;
        }
        else
        {
            // If this isn't a reassignment, we need to ensure that the value is unique
            ensure_unique_symbol(stmt.lhs);
        }

        /*
            Call the validation method to walk the assignment AST and build an instruction set for the 
            code generator while analyzing the data to ensure that all variables exist and to pull the type
            that the resulting operation would be
        */
        INTERMEDIATE::TYPES::AssignmentClassifier classification = INTERMEDIATE::TYPES::AssignmentClassifier::INTEGER; // Assume int 
        
        std::string postfix_expression = validate_assignment_ast(stmt.rhs, classification, stmt.data_type, stmt.lhs);

        // The unique value doesn't exist yet and needs some memory allocated and 
        // needs to be added to the symbol table
        if(requires_allocation_in_symbol_table)
        {
            // If this fails, we all fail. add_symbol will figure out data size 
            // and add to memory manager (Until we build more complicated structs
            // then we will have to update this call ) <<<<<<<<<<<<<<<<<<<<<<<<, TODO
            symbol_table.add_symbol(stmt.lhs, stmt.data_type);

            // No longer requires allocation
            requires_allocation_in_symbol_table = false;

            // Retrieve the generated memory information
            memory_info = memory_man.get_mem_info(stmt.lhs);
        }

        intermediate_layer.issue_assignment(stmt.lhs, memory_info, classification, postfix_expression);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::accept(ReturnStmt & stmt)
    {
        // Create a 'variable assignment' for the return so we can copy the value or whatever
        std::string variable_for_return = symbol_table.generate_unique_return_symbol();

        function_watcher.has_return = true;

        // Handle NIL / NONE Return
        if(stmt.data_type == ValType::NONE)
        {
            intermediate_layer.issue_null_return();
            return;
        }

        // Create an assignment for the return, this will execute the return withing code gen as we set a RETURN node type that is processed by the assignment
        Assignment * return_assignment = new Assignment(current_function->return_type, variable_for_return, new DEL::AST(DEL::NodeType::RETURN, stmt.rhs, nullptr));

        this->accept(*return_assignment);

        delete return_assignment;
    }

    // ----------------------------------------------------------
    // This is a call statment on its own, not in an expression
    // ----------------------------------------------------------

    void Analyzer::accept(Call & stmt)
    {
        validate_call(stmt);

        ValType callee_type = symbol_table.get_return_type_of_context(stmt.name);

        if(callee_type != ValType::NONE)
        {
            error_man.report_calls_return_value_unhandled(current_function->name, stmt.name);
        }

        // We endocde it to leverage the same functionality that is required by an expression-based call
        intermediate_layer.issue_direct_call(
            endecoder.encode_call(&stmt)
        );
    }

    // -----------------------------------------------------------------------------------------
    // 
    //                              Validation Methods
    //
    // -----------------------------------------------------------------------------------------

    void Analyzer::validate_call(Call & stmt)
    {
        // Disallow recursion until it is handled
        if(stmt.name == current_function->name)
        {
            error_man.report_custom("Analyzer", "Function recursion has not yet been implemented in Del", true);
        }

        // Ensure that the called method exists
        if(!symbol_table.does_context_exist(stmt.name))
        {
            error_man.report_callee_doesnt_exist(stmt.name);
        }

        // Get the callee params
        std::vector<FunctionParam> callee_params = symbol_table.get_context_parameters(stmt.name);

        // Ensure number of params match
        if(stmt.params.size() != callee_params.size())
        {
            error_man.report_mismatched_param_length(current_function->name, stmt.name, callee_params.size(), stmt.params.size());
        }

        // Ensure all paramters exist
        for(auto & p : stmt.params)
        {
            // If we need to get the type, get the type now that we know it exists
            if(p.type == ValType::REQ_CHECK)
            {
                // Ensure the thing exists, because REQ_CHECK dictates that the parameter is a variable, not a raw
                if(!symbol_table.does_symbol_exist(p.id))
                {
                    std::cerr << "Paramter in call to \"" << stmt.name << "\" does not exist in the current context" << std::endl;
                    error_man.report_unknown_id(p.id, true);
                }

                // Set the type to the type of the known variable
                p.type = get_id_type(p.id);
            }

            // If we didn't need to get the type, then it came in raw and we need to make a variable for it
            else
            {
                // Generate a unique label for the raw parameter
                std::string param_label = symbol_table.generate_unique_call_param_symbol();

                // Create an assignment for the variable
                Assignment * raw_parameter_assignment = new Assignment(p.type, param_label, 
                    new DEL::AST(DEL::NodeType::VAL, nullptr, nullptr, p.type, p.id)
                );

                this->accept(*raw_parameter_assignment);

                delete raw_parameter_assignment;

                if(!symbol_table.does_symbol_exist(param_label))
                {
                    std::cerr << "Auto generated parameter variable in call to \"" << stmt.name << "\" did not exist after assignment" << std::endl;
                    error_man.report_unknown_id(param_label); 
                }

                // If the call works, which it should, then we update the id to the name of the varaible
                // so we can reference it later
                p.id = param_label;
            }
        }

        // Check that the param types match
        for(uint16_t i = 0; i < stmt.params.size(); i++ )
        {
            if(stmt.params[i].type != callee_params[i].type)
            {
                std::cerr << "Parameter \"" << stmt.params[i].id << "\" does not match expected type in paramter list of function \"" << stmt.name << "\"" << std::endl;
                error_man.report_unallowed_type(stmt.params[i].id, true);
            }
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Analyzer::check_value_is_valid_for_assignment(ValType type_to_check, INTERMEDIATE::TYPES::AssignmentClassifier & c, ValType & et, std::string & id)
    {
        switch(type_to_check)
        {
            case ValType::STRING   :    error_man.report_custom("Analyzer", " STRING found in arithmetic exp",    true); // Grammar should have
            case ValType::REQ_CHECK:    error_man.report_custom("Analyzer", " REQ_CHECK found in arithmetic exp", true); // filteres these out
            case ValType::NONE     :    error_man.report_custom("Analyzer", " NONE found in arithmetic exp",      true);
            case ValType::FUNCTION :    error_man.report_custom("Analyzer", " FUNCTION found in arithmetic exp",  true);
            case ValType::REAL     : 
            {
                // Promote to Double if any double is present
                c = INTERMEDIATE::TYPES::AssignmentClassifier::DOUBLE;

                if((et != ValType::REAL) && (et != ValType::REAL)) 
                {
                    std::string error_message = id;

                    // There are better ways to do this, but if it happens at all it will only happen once during the compiler run
                    // as we are about to die
                    if(std::regex_match(id, std::regex("(__return__assignment__).*")))
                    {
                        error_message = "Function (" + current_function->name + ")";
                    } 
                    error_man.report_unallowed_type(error_message, true); 
                }

                break;
            }
            case ValType::INTEGER  :
            {
                // We assume its an integer to start with so we dont set type (because we allow ints inside double exprs)
                if(et == ValType::REAL) { error_man.report_unallowed_type(id, true); }
                break;
            }
            case ValType::CHAR     :
            {
                
                c = INTERMEDIATE::TYPES::AssignmentClassifier::CHAR;

                if(et == ValType::REAL)   { error_man.report_unallowed_type(id, true); } // If Assignee isn't a char, we need to die
                break;
            }
        }
    }

    // ----------------------------------------------------------
    // Assignee's expected type abbreviated to 'et' 
    // ----------------------------------------------------------

    std::string Analyzer::validate_assignment_ast(AST * ast, INTERMEDIATE::TYPES::AssignmentClassifier & c, ValType & et, std::string & id)
    {
        switch(ast->node_type)
        {
            case NodeType::ID  : 
            { 
                // Ensure the ID is within current context. Allowing any type
                ensure_id_in_current_context(ast->value, {});

                // Check for promotion
                ValType id_type = get_id_type(ast->value);

                // Make sure that the known value of the identifier is one valid given the current assignemnt
                check_value_is_valid_for_assignment(id_type, c, et, id);

                // Encode the identifier information so we can handle it in the intermediate layer
                return endecoder.encode_identifier(ast->value);
            }
            
            case NodeType::CALL :
            {
                // We know its a call, so lets treat it like a call
                Call * call = static_cast<Call*>(ast);

                // This call to validate_call will ensure that all parameters within the call exist in the system as variables
                // and it will update the current object to the new information we need to pull addresses
                validate_call(*call);

                // Ensure that the return type of the call is valid for the assignment
                // Our call to validate_call made sure that call->name exists as a context within symbol table
                // so we can use that value directly
                check_value_is_valid_for_assignment(
                    symbol_table.get_return_type_of_context(call->name)
                    , c, et, id
                );
                
                // Encode the call to something we can handle in the intermediate layer
                return endecoder.encode_call(call);
            }

            case NodeType::VAL : 
            { 
                // Check that the raw value is one that is valid within the current assignment
                check_value_is_valid_for_assignment(ast->val_type, c, et, id);
                return ast->value;
            }
            
            //  This is where we convert NodeType to the Assignment type. This should make it so we can change the actual tokens in the language
            //  without having to modify this statement

            case NodeType::ADD    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " ADD    " );  
            case NodeType::SUB    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " SUB    " );
            case NodeType::DIV    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " DIV    " );
            case NodeType::MUL    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " MUL    " );
            case NodeType::MOD    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " MOD    " );
            case NodeType::POW    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " POW    " );
            case NodeType::LTE    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " LTE    " );
            case NodeType::GTE    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " GTE    " );
            case NodeType::GT     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " GT     " );
            case NodeType::LT     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " LT     " );
            case NodeType::EQ     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " EQ     " );
            case NodeType::NE     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " NE     " );
            case NodeType::LSH    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " LSH    " );
            case NodeType::RSH    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " RSH    " );
            case NodeType::BW_OR  :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " BW_OR  " );
            case NodeType::BW_XOR :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " BW_XOR " );
            case NodeType::BW_AND :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " BW_AND " );
            case NodeType::OR     :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " OR     " );
            case NodeType::AND    :return (validate_assignment_ast(ast->l, c, et, id) + " " + validate_assignment_ast(ast->r, c, et, id) + " AND    " );
            case NodeType::BW_NOT :return (validate_assignment_ast(ast->l, c, et, id) + " BW_NOT ");
            case NodeType::NEGATE :return (validate_assignment_ast(ast->l, c, et, id) + " NEGATE "  );
            case NodeType::RETURN :return (validate_assignment_ast(ast->l, c, et, id) + " RETURN "  );
            case NodeType::ROOT   : error_man.report_custom("Analyzer", "ROOT NODE found in arithmetic exp", true); break;
            default:
            return "Its dead, jim";
        }
        return "Complete";
    }
}
