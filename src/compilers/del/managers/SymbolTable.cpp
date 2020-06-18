#include "SymbolTable.hpp"
#include <iostream>
namespace DEL
{

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    SymbolTable::SymbolTable(Errors & error_man, Memory & mm) : error_man(error_man),
                                                                memory_man(mm),
                                                                is_locked(false),
                                                                unique_counter(0)
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    SymbolTable::~SymbolTable()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void SymbolTable::new_context(std::string name, bool remove_previous)
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }

        if(remove_previous)
        {
            std::cout << "Contexts size : " << contexts.size() << std::endl;
            
            // Don't allow the removal of the global context
            if(contexts.size() > 1)
            {
                std::cout << "If you're seeing this, its the first time you've attempted to delete a context" << std::endl;

                Context * ciq = contexts.back();
                delete ciq;
                contexts.pop_back();
            }
        }

        contexts.push_back(new Context(name));
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    bool SymbolTable::does_symbol_exist(std::string symbol, bool show_if_found)
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }
        
        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->symbol_map.find(symbol) != c->symbol_map.end())
                {
                    if(show_if_found)
                    {
                        std::cout << "\"" << symbol 
                                  << "\" found in context \"" 
                                  << c->context_name 
                                  << "\" defined as : " 
                                  << ValType_to_string(c->symbol_map[symbol])
                                  << std::endl;
                    }
                    return true;
                }
            }
        }
        return false;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    bool SymbolTable::does_context_exist(std::string context)
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }
        
        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->context_name == context)
                {
                    return true;
                }
            }
        }
        return false;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::vector<FunctionParam> SymbolTable::get_context_parameters(std::string context)
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }

        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->context_name == context)
                {
                    return c->context_parameters;
                }
            }
        }
        error_man.report_custom("SymbolTable", "Developer error: Asked to get parameters from a context that did not exist", true);

        return std::vector<FunctionParam>(); // For the compiler
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void SymbolTable::clear_existing_context(std::string context)
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }
        
        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->context_name == context)
                {
                    c->symbol_map.clear();
                    return;
                }
            }
        }
        error_man.report_custom("SymbolTable", "Developer error: Asked to clear existing context, but that context did not exist", true);
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    bool SymbolTable::is_existing_symbol_of_type(std::string symbol, ValType type)
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }
        
        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->symbol_map.find(symbol) != c->symbol_map.end())
                {
                    if(c->symbol_map[symbol] == type)
                    {
                        return true;
                    }
                    else
                    {
                        return false;
                    }
                }
            }
        }
        return false;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void SymbolTable::add_symbol(std::string symbol, DEL::ValType type, uint64_t memory)
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }
        
        contexts.back()->symbol_map[symbol] = type;

        // A stop-gap to ensure we know why things break as stuff is expanded
        if(memory > 0 && memory != 8)
        {
            std::cerr << "You just attempted to allocate \"" << memory << "\" bytes of memory "
                      << "indicating that more complex types are being written. You're seeing this "
                      << "because codegen is not yet able to handle non-word aligned bytes. " 
                      << "This now needs to be supported. Thank you!";
            error_man.report_custom("SymbolTable", "Requires further development", true);
        }

        // Depending on the val_type we need to allocate some memory
        uint64_t mem_request = 0;
        switch(type)
        {
        case ValType::INTEGER:       mem_request = (memory == 0) ? SETTINGS::SYSTEM_WORD_SIZE_BYTES : memory; break; // Primitives always get a WORD by default
        case ValType::REAL:          mem_request = (memory == 0) ? SETTINGS::SYSTEM_WORD_SIZE_BYTES : memory; break; // Primitives always get a WORD by default
        case ValType::CHAR:          mem_request = (memory == 0) ? SETTINGS::SYSTEM_WORD_SIZE_BYTES : memory; break; // Primitives always get a WORD by default
        case ValType::FUNCTION:      error_man.report_custom("SymbolTable", " FUNCTION given to symbol table", true);
        case ValType::REQ_CHECK:     error_man.report_custom("SymbolTable", " REQ CHECK given to symbol table", true);
        case ValType::NONE:          return;
        case ValType::STRING:
        {
            // Strings and structs need to have a size given to us
            if(memory == 0) { error_man.report_custom("SymbolTable", "STRING added to symbol table without size", true); }
            mem_request = memory;
            break;
        }
        /*
        case ValType::STRUCT:
        {
            // Strings and structs need to have a size given to us
            if(memory == 0) { error_man.report_custom("SymbolTable", "STRUCT added to symbol table without size", true); }
            mem_request = memory;
            break;
        }
        */
        default:
            error_man.report_custom("SymbolTable", "Default reached in ValType matching", true);
            break;
        }

        // Safety check - If things are build correctly by compiler this should never happen
        if(memory_man.is_id_mapped(symbol))
        {
            std::cerr << "DEVELOPER ERROR : The symbol \" " << symbol << "\" was previously mapped within the memory manager" << std::endl;
            error_man.report_custom("SymbolTable", "Item given to symbol table already exists within the memory map. This is a developer error", true);
        }

        // Attempt to 'allocate' memory
        if(!memory_man.alloc_mem(symbol, mem_request))
        {
            error_man.report_out_of_memory(symbol, memory, Memory::MAX_GLOBAL_MEMORY);
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void SymbolTable::add_parameters_to_current_context(std::vector<FunctionParam> params)
    {
        contexts.back()->context_parameters.clear();
        contexts.back()->context_parameters = params;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    DEL::ValType SymbolTable::get_value_type(std::string symbol)
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }
        
        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->symbol_map.find(symbol) != c->symbol_map.end())
                {
                    return c->symbol_map[symbol];
                }
            }
        }
        return ValType::NONE;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void SymbolTable::add_return_type_to_current_context(DEL::ValType type)
    {
        contexts.back()->return_type = type;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    DEL::ValType SymbolTable::get_return_type_of_context(std::string context)
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }

        for(auto & c : contexts)
        {
            if(c != nullptr)
            {
                if(c->context_name == context)
                {
                    return c->return_type;
                }
            }
        }

        error_man.report_custom("SymbolTable", "Developer error: Asked to get parameters from a context that did not exist", true);

        return ValType::NONE; // For the compiler
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string SymbolTable::get_current_context_name() const
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }
        
        return contexts.back()->context_name;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string SymbolTable::generate_unique_return_symbol()
    {
        return generate_unique("__return__assignment__");
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string SymbolTable::generate_unique_call_param_symbol()
    {
        return generate_unique("__call__param__");
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string SymbolTable::generate_unique_variable_symbol()
    {
        return generate_unique("__artifical__variable__");
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    std::string SymbolTable::generate_unique(std::string start)
    {
        std::string unique_label = start + std::to_string(unique_counter);

        // Whis should never happen unless a user decides to use "__return__assignment__" as a variable name, which I guess we COULD deny, but why would we? 
        // its a perfectly good name.
        uint64_t stop_count = 0;
        while(does_symbol_exist(unique_label, false))
        {
            unique_counter++;
            unique_label = start + std::to_string(unique_counter);

            stop_count++;
            if(stop_count == 2048)
            {
                error_man.report_custom("SymbolTable", "2048 Attempts were made to generate a unique symbol. If you're seeing this, you're doing something silly. Stop it.", true);
            }
        }
        unique_counter++;

        return unique_label;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void SymbolTable::lock()
    {
        if(is_locked) { error_man.report_custom("SymbolTable", "Symbol table has been locked by the code generator", true); }
        
        is_locked = true;
    }
}