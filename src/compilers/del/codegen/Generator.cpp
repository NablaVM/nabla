#include "Generator.hpp"

#include "SystemSettings.hpp"

namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------
    
    Generator::Generator()
    {

    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------
    
    Generator::~Generator()
    {
        
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Generator::complete_code_generation(std::vector<std::string> & o)
    {
        // Bring in code for initialization
        asm_support.import_init_start(o);

        // Add space for where stack frame offset is stored
        o.push_back(".int64 __STACK_FRAME_OFFSET__\t" + std::to_string(SETTINGS::GS_INDEX_PROGRAM_SPACE) + "\n");

        // Add space in memory for parameter passing
        for(int i = 0; i < SETTINGS::GS_FUNC_PARAM_RESERVE; i++)
        {
            o.push_back(".int64 __PARAMETER__SPACE__" + std::to_string(i) + "__\t 0\n");
        }

        // Add reserved space
        for(int i = 0; i < SETTINGS::GS_RETURN_RESERVE; i++)
        {
            o.push_back(".int64 __RETURN_RESERVE__" + std::to_string(i) + "__\t 0\n");
        }

        // Add the start-up function
        asm_support.import_init_func(o);

        // Add the store / load functions
        asm_support.import_sl_funcs(o);

        // Add any built-in code that was triggerd to be added
        o.insert(o.end(), built_ins_triggered.begin(), built_ins_triggered.end());

        // Clear beacuse we're done
        built_ins_triggered.clear();

        // Add Instructions that the user has generated
        o.insert(o.end(), program_instructions.begin(), program_instructions.end());

        // Clear because we're done
        program_instructions.clear();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void Generator::add_instructions(std::vector<std::string> in_instructions)
    {
        program_instructions.insert(program_instructions.end(), in_instructions.begin(), in_instructions.end());
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------
    
    void Generator::include_builtin_math_pow(CODEGEN::TYPES::DataClassification classification, std::string & out_function_name)
    {
        if(classification == CODEGEN::TYPES::DataClassification::DOUBLE)
        {
            asm_support.import_math(AsmSupport::Math::POW_D, out_function_name, built_ins_triggered);

        }
        else
        {
            asm_support.import_math(AsmSupport::Math::POW_I, out_function_name, built_ins_triggered);
        }
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------
    
    void Generator::include_builtin_math_mod(CODEGEN::TYPES::DataClassification classification, std::string & out_function_name)
    {
        if(classification == CODEGEN::TYPES::DataClassification::DOUBLE)
        {
            asm_support.import_math(AsmSupport::Math::MOD_D, out_function_name, built_ins_triggered);

        }
        else
        {
            asm_support.import_math(AsmSupport::Math::MOD_I, out_function_name, built_ins_triggered);
        }
    }
}