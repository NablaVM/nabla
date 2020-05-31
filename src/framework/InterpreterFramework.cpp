#include "InterpreterFramework.hpp"

#include <iostream>

#include "nhll.hpp"
#include "nhll_driver.hpp"

namespace NABLA
{
    // ------------------------------------------------
    //
    // ------------------------------------------------

    InterpreterFramework::InterpreterFramework(NABLA::LibManifest & lm) : lib_manifest(lm)
    {

    }

    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    InterpreterFramework::~InterpreterFramework()
    {

    }

    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    int InterpreterFramework::interpret_project(NABLA::ProjectFS & project)
    {
        std::cout << "Interpreter Framework not yet setup for project interpretation" << std::endl;

        return 1;
    }

    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    int InterpreterFramework::interpret_cli()
    {
        std::cout << "Interpreter Framework not yet setup for cli interpretation" << std::endl;

        // Will need to run pre-processor on each line 
/*
        NHLL::CodeGen code_generator(lib_manifest);

        NHLL::NHLL_Driver driver(code_generator);
        driver.parse( std::cin );
*/
        return 1;
    }
}