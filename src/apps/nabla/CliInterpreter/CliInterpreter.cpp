#include "CliInterpreter.hpp"

// The libcpp interpreter
#include "interpreter.hpp"

#include <iostream>

namespace NABLA
{
    CliInterpreter::CliInterpreter() : prompt(">> "),
                                       doInterpretation(true),
                                       interpState(CliInterpreter::State::LINE)
    {

    }
    
    CliInterpreter::~CliInterpreter()
    {

    }

    int CliInterpreter::begin()
    {
        /*
        
            Need to handle ctrl^c 
        
        */
        while(doInterpretation)
        {
            std::string input;
            
            switch(interpState)
            {
                case CliInterpreter::State::LINE:
                {

                    std::cout << prompt;
                    getline(std::cin, input);
                    break;
                }

                case CliInterpreter::State::STATEMENT:
                {

                    // Will need to find a way to tab more depending on how deep in a statement block
                    // we are
                    std::cout << prompt << "\t";
                    getline(std::cin, input);
                    break;
                }

                case CliInterpreter::State::WAIT:
                {
                    break;
                }

            }
        }

        return 0;
    }
}