#ifndef NABLA_PARSER_HPP
#define NABLA_PARSER_HPP

#include "Tokens.hpp"

#include <functional>
#include <vector>

namespace NABLA
{
    class Parser
    {
    public:

        // Error information that can be sent back to the caller regarding a syntax error
        struct ErrorInformation
        {
            std::string errorMessage;
            uint64_t    lineNumber;
        };

        // Construct a parser and give it a callback to hand the detected tokens
        Parser(std::function<void(std::vector<TokenValue>)> parserCallback,
               std::function<void(ErrorInformation)>        errorCallback);

        // Scan a single line
        bool scanLine(std::string line);

        // Indicate that scan is completed
        void indicateComplete();

    private:
        std::function<void(std::vector<TokenValue>)>  callback;
        std::function<void(ErrorInformation)>         errorCallback;

        std::string lastLine;
        std::string currentLine;
        uint64_t    lineCount;
        std::vector<TokenValue> tokenValues;
        std::vector<std::string> errorMessages;

        bool matchNext(char c, int loc);
        bool matchPrev(char c, int loc);

        ErrorInformation createError(std::string message);

    };
}

#endif