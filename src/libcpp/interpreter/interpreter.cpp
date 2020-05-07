#include "interpreter.hpp"

namespace NABLA
{
    Interpreter::Interpreter() : ready(false),
                                 byte_cb(nullptr)
    {

    }

    Interpreter::~Interpreter()
    {

    }

    bool Interpreter::setupInterpreter(std::function<void(std::vector<uint8_t>)> byteCallback)
    {
        if(ready)
        {
            return false;
        }

        byte_cb = &byteCallback;

        ready = true;

        return true;
    }

    bool Interpreter::interpretLine(std::string line)
    {
        if(!ready)
        {
            return false;
        }

        // Parse the line

        return true;
    }

}