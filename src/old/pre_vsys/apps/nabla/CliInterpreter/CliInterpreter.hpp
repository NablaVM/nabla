#ifndef NABLA_CLI_INTERPRETER_HPP
#define NABLA_CLI_INTERPRETER_HPP

#include <string>

namespace NABLA
{
    //!
    //! \brief The Command line interface interpreter
    //!
    class CliInterpreter
    {
    public:
        //!
        //! \brief Construct a cli interpreter
        //!
        CliInterpreter();

        //!
        //! \brief Destroy cli interpreter
        //!
        ~CliInterpreter();

        //!
        //! \brief Start interpreting
        //!
        //! \retval Exit code for program
        //!
        int begin();

    private:

        enum class State
        {
            LINE,
            STATEMENT,
            WAIT
        };

        std::string prompt;
        bool doInterpretation;

        State interpState;
    };
}

#endif