#ifndef DEL_GENERATOR_HPP
#define DEL_GENERATOR_HPP

#include "AsmSupport.hpp"
#include "CodegenTypes.hpp"

#include <vector>
#include <string>

namespace DEL
{
    //! \class Generator
    //! \brief Used to handle generating the final resulting code, and provides commonly used
    //!        ASM generation functions
    class Generator
    {
    public:

        //! \brief Create the generator
        Generator();

        //! \brief Destruct the generator
        ~Generator();

        //! \brief Inidcate complete
        //! \param [out] output Vector to store resulting ASM
        void complete_code_generation(std::vector<std::string> & output);

        //! \brief Add a group of instructions to the final ASM output
        //! \param instructions The list of ASM instructions to be added
        void add_instructions(std::vector<std::string> instructions);

        //! \brief Indicate that we need to include the builtin POW function
        //! \param classification The type (INTEGER, DOUBLE) that the function must be able to accomodate
        //! \param out_function_name The name of the function that the caller can use to access the POW function
        void include_builtin_math_pow(CODEGEN::TYPES::DataClassification classification, std::string & out_function_name);

        //! \brief Indicate that we need to include the builtin MOD function
        //! \param classification The type (INTEGER, DOUBLE) that the function must be able to accomodate
        //! \param out_function_name The name of the function that the caller can use to access the MOD function
        void include_builtin_math_mod(CODEGEN::TYPES::DataClassification classification, std::string & out_function_name);

    private:
        AsmSupport asm_support;
        std::vector<std::string> built_ins_triggered;
        std::vector<std::string> program_instructions;
    };
}

#endif