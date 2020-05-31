#ifndef NABLA_INTERPRETER_FRAMEWORK_HPP
#define NABLA_INTERPRETER_FRAMEWORK_HPP

#include "CodeGen.hpp"

#include <string>
#include <libnabla/projectfs.hpp>

namespace NABLA
{
    //! \brief Framework for handling the cli->bytecode->execution routine
    class InterpreterFramework
    {
    public:
        //! \brief Create the Interpreter Framework intance
        InterpreterFramework(NABLA::LibManifest & lib_manifest);

        //! \brief Deconstruct the Interpreter Framework
        ~InterpreterFramework();

        //! \brief Interpret a nabla project
        //! \param project A reference to a loaded project
        //! \retval Application return code
        int interpret_project(NABLA::ProjectFS & project);
        
        //! \brief Command line interface interpreter
        //! \retbal Application return code
        int interpret_cli();

    private:
        NABLA::LibManifest & lib_manifest;
    };
}

#endif