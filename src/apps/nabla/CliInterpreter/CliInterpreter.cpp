#include "CliInterpreter.hpp"

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
        std::cout << "The CLI has not yet been implemented. This is a place holder." << std::endl;

        /*
        
            Need to handle ctrl^c 

            - The interpreter here will act as a manager for a 'virtual' project. Statements written will be dropped into
              the 'main' or 'entry' method. When we detect a function decl or something that needs to be exported from entry method, 
              once the user fully defines said method, it is manually placed external to entry method.
        
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