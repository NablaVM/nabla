#ifndef DEL_ASM_SUPPORT_HPP
#define DEL_ASM_SUPPORT_HPP

#include <map>
#include <vector>
#include <string>

namespace DEL
{
    //! \class AsmSupport
    //! \brief Plays a support role to include built-in methods that are written in assembly
    class AsmSupport
    {
    public:

        //! \brief The modules that the class can provide
        enum class Math
        {
            MOD_D = 0x00,
            MOD_I = 0x01,
            POW_D = 0x02,
            POW_I = 0x03
        };

        //! \brief Construct the class
        AsmSupport();

        //! \brief Destruct the class
        ~AsmSupport();

        //! \brief Import the ASM code to initialize the ASM file
        //! \param destination [out] The vector to place the code
        void import_init_start(std::vector<std::string> & destination);

        //! \brief Import the ASM code to enter into on initialization
        //! \param destination [out] The vector to place the code
        void import_init_func(std::vector<std::string> & destination);

        //! \brief Import a math module
        //! \param math_import The module to import
        //! \param function_name_out [out] The name of the function the caller can use to access the imported function
        //! \param destination [out] The vector to place the code
        //! \note This method can be called as much as you want, it will only ever import one copy of the module requested
        void import_math(AsmSupport::Math math_import, std::string & function_name_out, std::vector<std::string> & destination);

    private:

        struct ImportInfo
        {
            bool imported;
            std::string function_name;
            std::string function;
        };

        struct InitImport
        {
            bool start;
            bool func;
        };
        InitImport init_import;

        std::map<Math, ImportInfo> math_imports;
    };
}

#endif