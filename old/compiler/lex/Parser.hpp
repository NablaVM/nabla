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

        // Sets 'token' to the next token found. Returns -1 if token not found,
        // otherwise it will return the length of the token
        int getNextToken(int startPos, TokenValue & token);

        // Check if previous token is real, integer or variable
        bool isPreviousTokenPrimitiveOrVariable();
        bool isGivenTokenPrimitiveOrVariable(TokenValue token);

        // Build a potential number
        int buildFromDigit(int startPost, TokenValue & token);

        // Tokens that could be arithmatic operation, or they could be a comment, etc
        int buildPotentialOp(int startPos, TokenValue & token);

        // Check for reserved keywords
        int scanForKeywords(int startPos, TokenValue & token);

        // Check for variable
        int scanForVariable(int startPos, TokenValue & token);

        // Check for a function call
        int scanForFunctionCall(int startPos, TokenValue & token);

        ErrorInformation createError(std::string message);

    };
}

#endif