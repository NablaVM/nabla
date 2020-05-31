#include "CompilerFramework.hpp"

#include <iostream>

#include "Preprocessor.hpp"
#include "CodeGen.hpp"
#include "nhll.hpp"
#include "nhll_driver.hpp"

namespace NABLA
{
    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    CompilerFramework::CompilerFramework(NABLA::LibManifest & lm) : lib_manifest(lm)
    {

    }

    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    CompilerFramework::~CompilerFramework()
    {

    }

    // ------------------------------------------------
    //
    // ------------------------------------------------
    
    int CompilerFramework::compile_project(NABLA::ProjectFS & project)
    {
        std::cout << "Compile framework not yet setup" << std::endl;

        NHLL::Preprocessor preproc(lib_manifest, true);

        if(!preproc.process(project))
        {
            return 1;
        }

        // Get output from preproc for project and feed to driver

        NHLL::CodeGen code_generator;

        NHLL::NHLL_Driver driver(code_generator);

        //
        //  This will all change once the compiler is completed
        //
        NABLA::ProjectStructure ps = project.get_project_structure();
        
        driver.parse( preproc.get_preproc().c_str() );

        //driver.parse( theFile.nhll );

        //driver.print( std::cout ) << "\n";
        return 1;
    }
}