#ifndef NABLA_COMPILER_FRAMEWORK_HPP
#define NABLA_COMPILER_FRAMEWORK_HPP

#include <string>
#include "LibManifest.hpp"
#include <libnabla/projectfs.hpp>

namespace NABLA
{
    //! \brief Framework for handling the compilation of a project
    class CompilerFramework
    {
    public:
        //! \brief Create the Compiler Framework intance
        CompilerFramework(NABLA::LibManifest & lib_manifest);

        //! \brief Deconstruct the Interpreter Framework intance
        ~CompilerFramework();

        //! \brief Compile a nabla project
        //! \param project A reference to a loaded project
        //! \retval Application return code
        int compile_project(NABLA::ProjectFS & project);
    private:
        NABLA::LibManifest & lib_manifest;
    };
}

#endif