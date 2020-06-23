#ifndef DEL_CODE_GEN_HPP
#define DEL_CODE_GEN_HPP

#include "Ast.hpp"
#include "Errors.hpp"
#include "SymbolTable.hpp"
#include "Memory.hpp"
#include "Intermediate.hpp"

#include "CodegenTypes.hpp"
#include "Generator.hpp"
#include "Function.hpp"

#include <stack>

namespace DEL
{
    //! \brief Code generator
    class Codegen
    {
    public:
    
        //! \brief Construct a code generator
        //! \param err The error manager
        //! \param symbolTable The program symbol table
        //! \param memory The memory manager
        Codegen(Errors & err, SymbolTable & symbolTable, Memory & memory);

        //! \brief Deconstrut the code generator
        ~Codegen();

        //! \brief Complete the generation of code
        //! \retval ASM generated by codegen
        std::vector<std::string> indicate_complete();

        //! \brief Begin function
        //! \param name The function name
        //! \param params The function's parameters
        //! \post The code generator will be set to start building a function
        void begin_function(std::string name, std::vector<CODEGEN::TYPES::ParamInfo> params);

        //! \brief End function
        //! \post The code generator will be ready to begin another function
        void end_function();

        //! \brief Mark the start of a conditional  (if)
        //! \param conditional_init The information for the initiation of a conditional
        void begin_conditional(CODEGEN::TYPES::ConditionalInitiation conditional_init);

        //! \brief Extend the current conditional with a chained statement (elseif/else)
        //! \param coniditional_init The information for the next initiiation of a conditional
        void extend_conditional(CODEGEN::TYPES::ConditionalInitiation conditional_init);

        //! \brief End a conditional
        void end_conditional();

        //! \brief Begin a loop
        //! \param loop_if The loop
         void begin_loop(CODEGEN::TYPES::LoopIf * loop_if);

        //! \brief End while loop
        //! \param type What type of loop is being ended
        void end_loop(CODEGEN::TYPES::LoopType type);

        //! \brief Generate something based on a command
        //! \param command The instructions used to generate code
        void execute_command(CODEGEN::TYPES::Command command);

        //! \brief Create a special return that doesn't return a value
        void null_return();

    private:
        Errors & error_man;         // Error manager
        SymbolTable & symbol_table; // Symbol table
        Memory & memory_man;
        bool building_function;

        uint64_t label_id;

        Generator generator;

        std::vector<std::string> program_init;
        std::vector<std::string> program_instructions;

        // A function being generated (an aggregator)
        CODE::Function * current_function;

        // Directs block generation to destination for aggregation
        CODE::BlockAggregator * current_aggregator;

        // As conditional statements come in they are pushed into this stack
        // so we can have nested conditionals handled as they need to be.
        // ConditionalContext objects are BlockAggregators
        std::stack<CODE::BlockAggregator*> aggregators;
    };
}

#endif 