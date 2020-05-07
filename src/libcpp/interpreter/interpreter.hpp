#ifndef NABLA_INTERPRETER_HPP
#define NABLA_INTERPRETER_HPP

#include <vector>
#include <stdint.h>
#include <string>
#include <functional>

namespace NABLA
{
/*
    Interpreter takes in HLL text and once enough has been put in to complete a segment, a callback
    will be fired containing ready-to-(exeecute|load) code. 

    let int:a = 3;  // Instantly ready

    while(a)        // not ready
    {               // not ready
    }               // ready

*/
    class Interpreter
    {
    public:
        Interpreter();
        ~Interpreter();

        bool setupInterpreter(std::function<void(std::vector<uint8_t>)> byteCallback);

        bool interpretLine(std::string line);

    private:
        bool ready;

        std::function<void(std::vector<uint8_t>)> * byte_cb;
    };
}

#endif