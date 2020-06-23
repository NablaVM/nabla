#include "AsmSupport.hpp"

#include "AsmInit.hpp"
#include "AsmMath.hpp"
#include "AsmStoreLoad.hpp"

#include <iostream>
namespace DEL
{
    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    AsmSupport::AsmSupport()
    {
        init_import = AsmSupport::InitImport{ false, false };
        math_imports[AsmSupport::Math::MOD_D] = AsmSupport::ImportInfo{ false, BUILT_IN::ASM_MOD_D_FUNCTION_NAME, BUILT_IN::ASM_MOD_D  };
        math_imports[AsmSupport::Math::MOD_I] = AsmSupport::ImportInfo{ false, BUILT_IN::ASM_MOD_FUNCTION_NAME  , BUILT_IN::ASM_MOD    };
        math_imports[AsmSupport::Math::POW_D] = AsmSupport::ImportInfo{ false, BUILT_IN::ASM_POW_D_FUNCTION_NAME, BUILT_IN::ASM_POW_D  };
        math_imports[AsmSupport::Math::POW_I] = AsmSupport::ImportInfo{ false, BUILT_IN::ASM_POW_FUNCTION_NAME  , BUILT_IN::ASM_POW    };
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    AsmSupport::~AsmSupport()
    {
        math_imports.clear();
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void AsmSupport::import_init_start(std::vector<std::string> & destination)
    {
        if(init_import.start){ return; }

        destination.push_back(BUILT_IN::ASM_FILE_START);

        init_import.start = true;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void AsmSupport::import_init_func(std::vector<std::string> & destination)
    {
        if(init_import.func){ return; }

        destination.push_back(BUILT_IN::ASM_INIT_FUNCTION);

        init_import.func = true;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void AsmSupport::import_sl_funcs(std::vector<std::string> & destination)
    {
        if(init_import.store_load){ return; }

        destination.push_back(BUILT_IN::ASM_ALLOC);
        destination.push_back(BUILT_IN::ASM_FREE);
        destination.push_back(BUILT_IN::ASM_LOAD);
        destination.push_back(BUILT_IN::ASM_STORE);

        init_import.store_load = true;
    }

    // ----------------------------------------------------------
    //
    // ----------------------------------------------------------

    void AsmSupport::import_math(AsmSupport::Math math_import, std::string & function_name_out, std::vector<std::string> & destination)
    {
        // Only include the function once
        if(!math_imports[math_import].imported)
        {
            destination.push_back(math_imports[math_import].function);

            math_imports[math_import].imported = true;
        }

        // Always set the name
        function_name_out = math_imports[math_import].function_name;
    }
}