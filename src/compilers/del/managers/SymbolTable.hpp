#ifndef DEL_SYMBOL_TABLE_HPP
#define DEL_SYMBOL_TABLE_HPP

#include <vector>
#include <map>
#include <string>
#include "Ast.hpp"
#include "Memory.hpp"
#include "Errors.hpp"

namespace DEL
{
    // Fwd for friendship
    class Codegen;

    //!\class SymbolTable
    //!\brief A symbol table for keeping track of variables and contexts
    class SymbolTable
    {
    public:
        //!\brief Setup a symbol table
        //!\param error_man The error manager
        //!\param memory_man The memory manager
        SymbolTable(Errors & error_man, Memory & memory_man);

        //!\brief Destruct table
        ~SymbolTable();

        //! \brief Create a new named context
        //! \param name The name of the context
        //! \param remove_previous If true, the current context will be removed from knowledge base
        void new_context(std::string name, bool remove_previous=false);

        //! \brief Check if a symbol exists
        //! \param symbol The symbol to check for
        //! \param show_if_found Displays symbol information if true
        //! \retval True if exists, false otherwise
        bool does_symbol_exist(std::string symbol, bool show_if_found=false);

        //! \brief Check if a context exits
        //! \param context The context name to check for
        //! \retval True if exists, false otherwise
        bool does_context_exist(std::string context);

        //! \brief Check if a symbol is a given type
        //! \param symbol The symbol to check
        //! \param type The type to check
        //! \retval True if is type, false otherwise (including if the symbol doesn't exist)
        bool is_existing_symbol_of_type(std::string symbol, ValType type);

        //! \brief Add a symbol to the current context
        //! \param symbol The symbol to add
        //! \param type The value type to add as
        //! \param memory How much memory to ask for, if 0 (default) the default amount
        //!        of memory for the type will be asked for, if there is a default
        void add_symbol(std::string symbol, DEL::ValType type, uint64_t memory=0);

        //! \brief Add parameters to the current context
        //! \param params The parameters to add to the context
        //! \note  This assumes that parameters don't yet exist. If they do, they'll be overwritten
        void add_parameters_to_current_context(std::vector<FunctionParam> params);

        //! \brief Get the parameters for a given context
        //! \param context The name of the context to get the parameters of
        std::vector<FunctionParam> get_context_parameters(std::string context);

        //! \brief Add a return type to the current context
        //! \param type The type that the context is expected to return
        void add_return_type_to_current_context(ValType type);

        //! \brief Get the return type of the given context
        //! \param context The context name to get the return type of
        ValType get_return_type_of_context(std::string context);

        //! \brief Clear the existing context of all symbols
        //! \param context The context to clear
        void clear_existing_context(std::string context);

        //! \brief Retrieve the data type of a symbol
        //! \param symbol The symbol to check for
        //! \returns Type of the given symbol, DEL::ValType::NONE if it doesn't exist
        DEL::ValType get_value_type(std::string symbol);

        //! \brief Get the name of the current context
        //! \returns String of the current context name
        std::string get_current_context_name() const;

        //! \brief Generate a unique symbol for a return
        //! \returns Unique symbol to use as a return item in assignment
        std::string generate_unique_return_symbol();

        //! \brief Generate a unique symbol for a raw item in a function call
        //! \returns Unique symbol to use as a call parameter in a call
        std::string generate_unique_call_param_symbol();

        friend Codegen;

    private:

        Errors & error_man;
        Memory & memory_man;
        
        bool is_locked;
        uint64_t unique_counter;

        // This is a silly thing I added while we develop so when the codegen is told we're done it uses this to 
        // lock functionality. Once we get things done this should be removed.
        void lock();

        std::string generate_unique(std::string start);

        class Context
        {
        public:
            Context(std::string name) : context_name(name), return_type(ValType::NONE) {}
            std::string context_name;
            std::map< std::string, DEL::ValType > symbol_map; 
            std::vector<FunctionParam> context_parameters;
            ValType return_type;
        };

        std::vector<Context * > contexts; 
    };
}

#endif