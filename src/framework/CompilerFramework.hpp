#ifndef NABLA_COMPILER_FRAMEWORK_HPP
#define NABLA_COMPILER_FRAMEWORK_HPP

#include <string>

namespace NABLA
{
    //! \brief Framework for handling the compilation of a project
    class CompilerFramework
    {
    public:
        //! \brief Create the Compiler Framework intance
        CompilerFramework();

        //! \brief Deconstruct the Interpreter Framework intance
        ~CompilerFramework();

        //! \brief Kick-off compilation given a file path
        int compile(std::string path);

    private:
        //NABLA::LibManifest & lib_manifest;
    };
}

#endif