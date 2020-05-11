/*
    A machine that is loaded from a compiled binary file
*/

#ifndef NABLA_VSYS_LOADABLE_MACHINE
#define NABLA_VSYS_LOADABLE_MACHINE

#include "VSysMachine.hpp"

#include <string>

namespace NABLA
{
namespace VSYS
{
    class LoadableMachine : public Machine
    {
    public:

        enum class ResultCodes
        {
            OKAY
        };

        LoadableMachine();
        ~LoadableMachine();

        ResultCodes loadFile(std::string path);

    private:

    };
}
}

#endif 