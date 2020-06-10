#include "CompilerFramework.hpp"

#include <iostream>

#include "del_driver.hpp"

namespace NABLA
{
    // ------------------------------------------------
    //
    // ------------------------------------------------

    CompilerFramework::CompilerFramework()
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

    int CompilerFramework::compile(std::string path)
    {
        DEL::DEL_Driver driver;

        driver.parse(path.c_str());

        return 0;
    }
}