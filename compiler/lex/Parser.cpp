#include "Parser.hpp"

#include <iostream>
#include <ctype.h>
#include <cctype>
#include <string>
#include <stdlib.h>  

namespace NABLA
{
    namespace
    {
        constexpr int RETURN_SKIP_TOKEN       =   0;
        constexpr int RETURN_SKIP_REMAINING   =  -1;
        constexpr int RETURN_NOT_FOUND        =  -2;
        constexpr int RETURN_ERROR            = -10;

        bool matchesKeywordString(std::string check)
        {
            if(check == "def" || check == "int" || check == "real" || check == "str" || check == "vec" ||
               check == "mat" || check == "emit"|| check == "pub")
               {
                   return true;
               }

            return false;
        }

        bool isWsChar(char suspect)
        {
            return std::isspace(static_cast<unsigned char>(suspect));
        }
    }



    // -------------------------------------------------------
    // Parser
    // -------------------------------------------------------

    Parser::Parser(std::function<void(std::vector<TokenValue>)> cb,
                   std::function<void(ErrorInformation)>        ecb) : callback(cb),
                                                                        errorCallback(ecb),
                                                                        lastLine(""),
                                                                        currentLine(""),
                                                                        lineCount(0)
    {
        
    }

    // -------------------------------------------------------
    // indicateComplete
    // -------------------------------------------------------

    void Parser::indicateComplete()
    {
        if(tokenValues.size() > 0)
        {
            callback(tokenValues);
            tokenValues.clear();
        }
    }

    // -------------------------------------------------------
    // matchNext
    // -------------------------------------------------------

    bool Parser::matchNext(char c, int loc)
    {
        if(loc == currentLine.size())
        {
            return false;
        }

        if(currentLine[loc] == '\0')
        {
            return false;
        }

        if(currentLine[loc] == c)
        {
            return true;
        }

        return false;
    }

    // -------------------------------------------------------
    // matchPrev
    // -------------------------------------------------------

    bool Parser::matchPrev(char c, int loc)
    {

        return false;
    }

    // -------------------------------------------------------
    // isPreviousTokenPrimitiveOrVariable
    // -------------------------------------------------------
    
    bool Parser::isPreviousTokenPrimitiveOrVariable()
    {
        if(tokenValues.size() == 0) { return false; }

        return isGivenTokenPrimitiveOrVariable(tokenValues.back());
    }

    // -------------------------------------------------------
    // isGivenTokenPrimitiveOrVariable - Used in other places than above method
    // -------------------------------------------------------

    bool Parser::isGivenTokenPrimitiveOrVariable(TokenValue tokenValue)
    {
        if(tokenValue.token == Token::REAL)     { return true; }
        if(tokenValue.token == Token::INTEGER)  { return true; }
        if(tokenValue.token == Token::VARIABLE) { return true; }
        return false;
    }

    // -------------------------------------------------------
    // createError
    // -------------------------------------------------------

    Parser::ErrorInformation Parser::createError(std::string message)
    {
        ErrorInformation ei;
        ei.errorMessage = message;
        ei.lineNumber   = lineCount;
        return ei;
    }

    // -------------------------------------------------------
    // scanLine
    // -------------------------------------------------------

    bool Parser::scanLine(std::string line)
    {
        lineCount++;
        currentLine = line;

        /*
        
            Go through the length of the string. For every char we call getNextToken. 
            Get next token will attempt to find a token from this position. If it finds a token,
            it will return the length of the token, and the tokenvalue. In the good case, we save the
            tokenvalue and step through the string the entire length of the token.

            If an error occurs, RETURN_ERROR will come in, and we return false. 
            If RETURN_SKIP_TOKEN comes in, it means we've come across a char we need to skip (\r \t ' ', etc)
            If RETURN_SKIP_REMAINING comes in, we probably hit a comment '//'
        
        */
        for(int i = 0; i < currentLine.length(); )
        {
            TokenValue currentToken;

            int   result  = getNextToken(i, currentToken);

            // Skip the current token for some reason
            if(RETURN_SKIP_TOKEN == result)
            {
                i++;
            }

            // There was a parse error, so this line fails
            else if (RETURN_ERROR == result)
            {
                return false;
            }

            // Something in code that makes us skip the rest of the line (most likely a comment)
            else if (RETURN_SKIP_REMAINING == result)
            {
                return true;
            }

            // Something good 
            else 
            {
                i += result;
                tokenValues.push_back(currentToken);
            }
        }
        
        return true;
    }

    // -------------------------------------------------------
    // getNextToken
    // -------------------------------------------------------

    int Parser::getNextToken(int startPos, TokenValue & token)
    {
        if(isWsChar(currentLine[startPos]))
        {
            return RETURN_SKIP_TOKEN;
        }

        switch (currentLine[startPos])
        {
        // -------------------------------------------------------------------------------------
        //  Statement end
        // -------------------------------------------------------------------------------------
        case ';': 
            // Send tokens off and clear the vector
            indicateComplete();
            return 1;

        // -------------------------------------------------------------------------------------
        //  +
        // -------------------------------------------------------------------------------------
        case '+': 
        {
            return buildPotentialOp(startPos, token);
        }

        // -------------------------------------------------------------------------------------
        //  -
        // -------------------------------------------------------------------------------------
        case '-': 
        {
            return buildPotentialOp(startPos, token);
        }
        // -------------------------------------------------------------------------------------
        //  *
        // -------------------------------------------------------------------------------------
        case '*': 
        {
            return buildPotentialOp(startPos, token);
        }
        // -------------------------------------------------------------------------------------
        //  /
        // -------------------------------------------------------------------------------------
        case '/': 
        {
            return buildPotentialOp(startPos, token);
        }
        // -------------------------------------------------------------------------------------
        //  =
        // -------------------------------------------------------------------------------------
        case '=': 
        {
            return buildPotentialOp(startPos, token);
        }

        // -------------------------------------------------------------------------------------
        //  Open Paren
        // -------------------------------------------------------------------------------------
        case '(': 
        {
            bool closeFound = false;
            for(int c = startPos; c < currentLine.size() && !closeFound; c++)
            {
                if(currentLine[c] == ')') { closeFound = true; }
            }

            if(closeFound)
            {
                token = TokenValue{ Token::L_PAREN, "(" }; 
            }
            else 
            {
                errorCallback(createError("Syntax Error. No matching ')' found."));
                return RETURN_ERROR;
            }

            return 1;
        }

        // -------------------------------------------------------------------------------------
        //  Closed Paren
        // -------------------------------------------------------------------------------------
        case ')': 
        {
            if(! isPreviousTokenPrimitiveOrVariable() && 
                 tokenValues.back().token != Token::R_PAREN)
            {
                errorCallback(createError("Syntax Error. No item precedes ')'"));
                return RETURN_ERROR;
            }

            bool openFound = false;
            for(int c = startPos; c >= 0 && !openFound; c--)
            {
                if(currentLine[c] == '(') { openFound = true; }
            }

            if(openFound)
            {
                token = TokenValue{ Token::R_PAREN, ")" };
            }
            else 
            {
                errorCallback(createError("Syntax Error. No matching '(' found."));
                return RETURN_ERROR;
            }

            return 1;
        }

        // -------------------------------------------------------------------------------------
        //  Everything else
        // -------------------------------------------------------------------------------------
        default:

            // ----------------------------------------------------------------------------------
            //  Locate a number ( 42 | 42.22 )
            // ----------------------------------------------------------------------------------
            if(isdigit(currentLine[startPos]))
            {
                return buildFromDigit(startPos, token);
            }

            // ----------------------------------------------------------------------------------
            //  Scan for keywords. If a keyword is found, their build method will be called
            //  and all errors ___SHOULD___ be piped through nicely. 
            //  The only reason we don't go direct is so we don't RETURN_NOT_FOUND
            // ----------------------------------------------------------------------------------
            int keyWordScan = scanForKeywords(startPos, token);
            if(keyWordScan != RETURN_NOT_FOUND)
            {
                return keyWordScan;
            }

            // ----------------------------------------------------------------------------------
            //  Locate a variable
            // ----------------------------------------------------------------------------------

            int variableScan = scanForVariable(startPos, token);
            if(variableScan != RETURN_NOT_FOUND)
            {
                return variableScan;
            }


            // ----------------------------------------------------------------------------------
            //  Everything else should be an error
            // ----------------------------------------------------------------------------------

            std::string e = "Error. Unexpected value: [";  e += currentLine[startPos]; e += "] found";
            errorCallback(createError(e));
            return RETURN_ERROR;
            break;
        }

        return RETURN_ERROR;
    }

    // -------------------------------------------------------
    // Build something from a digit
    // -------------------------------------------------------

    int Parser::buildFromDigit(int startPos, TokenValue & token)
    {
        // Assume it is an integer until a '.' is found
        bool isReal = false;

        // Accumulate the numbers
        std::string digits; digits += currentLine[startPos];

        // Accumulate the steps
        int currIdx = 1;

        // Scan up-to the rest of the line for the remaining parts of the number
        for(int c = startPos+1; c < currentLine.size(); c++)
        {
            // If its a digit, add it 
            if(isdigit(currentLine[c]))
            {
                digits += currentLine[c];
                currIdx++;
            }

            // If its a '.' it can be assumed that its a 'real'
            else if(currentLine[c] == '.')
            {
                isReal = true;

                // Ensure that the next item is a digit
                if(c + 1 == currentLine.size() || !isdigit(currentLine[c+1]) )
                {
                    errorCallback(createError("Syntax error. Expected a number following : " + digits + ". "));
                    return RETURN_ERROR;
                }

                digits += currentLine[c];
                currIdx++;
            }
            else
            {
                c = currentLine.size(); // Kill the loop
            }
        } // End inner search


        if (isReal)
        {
            token = TokenValue{Token::REAL, digits};
        }
        else 
        {
            token = TokenValue{Token::INTEGER, digits};
        }
        
        return currIdx;
    }

    // -------------------------------------------------------
    // buildPotentialOp
    //
    //  Potential ops key off of tokens that are prefixed with 'OP_'
    //  Each of these ops need to have L and R checked for [ variable | real | int | ... primitives ]
    //  in at least one of their cases 
    //
    // -------------------------------------------------------

    int Parser::buildPotentialOp(int startPos, TokenValue & token)
    {
        int tokenSize;
        TokenValue currentOpToken;

        switch(currentLine[startPos])
        {
            // -------------------------------------------------------------------------------------
            //  +
            // -------------------------------------------------------------------------------------
            case '+': 
            {
                if(matchNext('=', startPos+1))
                {
                    currentOpToken = TokenValue{ Token::OP_ADDEQ, "+="};
                    tokenSize = 2;
                }
                else
                {
                    currentOpToken = TokenValue{ Token::OP_ADD, "+"};
                    tokenSize = 1;
                }
                break;
            }

            // -------------------------------------------------------------------------------------
            //  -
            // -------------------------------------------------------------------------------------
            case '-':
            {
                if(matchNext('=', startPos+1))
                {
                    currentOpToken = TokenValue{ Token::OP_SUBEQ, "-="};
                    tokenSize = 2;
                }
                else
                {
                    currentOpToken = TokenValue{ Token::OP_SUB, "-"};
                    tokenSize = 1;
                }
                break;
            }

            // -------------------------------------------------------------------------------------
            //  *
            // -------------------------------------------------------------------------------------
            case '*': 
            {
                if(matchNext('=', startPos+1))
                {
                    currentOpToken = TokenValue{ Token::OP_MULEQ, "*="};
                    tokenSize = 2;
                }
                else
                {
                    currentOpToken = TokenValue{ Token::OP_MUL, "*"};
                    tokenSize = 1;
                }
                break;
            }
            
            // -------------------------------------------------------------------------------------
            //  / Could be a comment, could be a div could be a diveq
            // -------------------------------------------------------------------------------------
            case '/':
            {
                if(matchNext('/', startPos+1))
                {
                    // A comment was found. Ignore the lines
                    return RETURN_SKIP_REMAINING;
                }
                
                if(matchNext('=', startPos+1))
                {
                    currentOpToken = TokenValue{ Token::OP_DIVEQ, "/="};
                    tokenSize = 2;
                }
                else
                {
                    currentOpToken = TokenValue{ Token::OP_DIV, "/"};
                    tokenSize = 1;
                }
                break;
            }

            // -------------------------------------------------------------------------------------
            //  =
            // -------------------------------------------------------------------------------------
            case '=':
            {
                if(matchNext('=', startPos+1))
                {
                    currentOpToken = TokenValue{ Token::OP_COMPARE,  "==" }; 
                    tokenSize = 2;
                }
                else
                {
                    currentOpToken = TokenValue{ Token::OP_ASSIGN,  "=" }; 
                    tokenSize = 1;
                }
                break;
            }

        }

        // Ensure that the tokens to the L and R of the op are valid
        //
        // ------------------------------------------------------------
        
        if(!isPreviousTokenPrimitiveOrVariable())
        {
            errorCallback(createError("Expected an int, real, or variable preceding given op"));
            return RETURN_ERROR;
        }

        // Ensure we aren't at the end of the line
        if(startPos + tokenSize >= currentLine.size())
        {
            errorCallback(createError("Expected an int, real, or variable succeeding given op, instead we found EOL"));
            return RETURN_ERROR;
        }

        bool findNextToken = true;

        int startFind = tokenSize;

        while(findNextToken)
        {
            TokenValue nextToken;
            int nextTokenResult = getNextToken(startPos+startFind, nextToken);

            if(nextTokenResult == RETURN_ERROR || nextTokenResult == RETURN_SKIP_REMAINING) 
            {
                errorCallback(createError("Expected an int, real, or variable succeeding given op"));
                return RETURN_ERROR;
            }

            if(nextTokenResult == RETURN_SKIP_TOKEN)
            {
                startFind++;
                continue;
            }

            // Ensure RHS of token is a prim, var, or '('
            if(isGivenTokenPrimitiveOrVariable(nextToken))
            {
                findNextToken = false;
            }
            else if(nextToken.token == Token::L_PAREN)
            {
                findNextToken = false;
            }
            else
            {
                errorCallback(createError("Expected an int, real, or variable succeeding given op"));
                return RETURN_ERROR;
            }
        }

        // At this point the tokens to the L and R are valid
        // ------------------------------------------------------------

        token = currentOpToken;
        return tokenSize;
    }

    // -------------------------------------------------------
    // scanForKeywords
    // -------------------------------------------------------

    int Parser::scanForKeywords(int startPos, TokenValue & token)
    {
        // ----------------------------------------------------
        //  Build an int declaration
        // ----------------------------------------------------
        if( currentLine[startPos] == 'i' &&
            matchNext('n', startPos+1)   && 
            matchNext('t', startPos+2)   && 
            matchNext(':', startPos+3))
        {
            // If we are in a a function definition we need to check that
            // the next token is a',' or a ') -> type {' for now we are not
            // doing that

            // Ensure R is VARIABLE

            // End of line makes this invalid now
            if(currentLine.size() <= startPos+4)
            {
                return RETURN_ERROR;
            }

            TokenValue variableVar;

            if(RETURN_NOT_FOUND == scanForVariable(startPos+4, variableVar))
            {
                errorCallback(createError("No variable name given for variable decl"));
                return RETURN_ERROR;
            }

            token.token = Token::INT_DECL;
            token.value = "int:";
            return 4;
        }

        // ----------------------------------------------------
        //  Build a str declaration
        // ----------------------------------------------------
        if( currentLine[startPos] == 's' &&
            matchNext('t', startPos+1)   && 
            matchNext('r', startPos+2)   && 
            matchNext(':', startPos+3))
        {
            if(currentLine.size() <= startPos+4)
            {
                return RETURN_ERROR;
            }

            TokenValue variableVar;

            if(RETURN_NOT_FOUND == scanForVariable(startPos+4, variableVar))
            {
                errorCallback(createError("No variable name given for variable decl"));
                return RETURN_ERROR;
            }

            token.token = Token::STR_DECL;
            token.value = "str:";
            return 4;
        }


        // ----------------------------------------------------
        //  Build a real declaration
        // ----------------------------------------------------
        if( currentLine[startPos] == 'r' &&
            matchNext('e', startPos+1)   && 
            matchNext('a', startPos+2)   && 
            matchNext('l', startPos+2)   && 
            matchNext(':', startPos+3))
        {

            #warning not done
        }

        return RETURN_NOT_FOUND;
    }

    // -------------------------------------------------------
    // scanForVariable
    // -------------------------------------------------------

    int Parser::scanForVariable(int startPos, TokenValue & token)
    {
        int variableLength = 0;

        // variables cant start with a digit
        if(isdigit(currentLine[startPos]))
        {
            return RETURN_NOT_FOUND;
        }

        std::string variableName;

        bool scanForEnd = true;
        while(scanForEnd)
        {
            // Allow ws before actual variable
            if(variableLength == 0 && isWsChar(currentLine[startPos]))
            {
                startPos++;
                continue;
            }

            // If we are at the end and still scanning, we can't count it as a 
            // valid var
            if(startPos + variableLength > currentLine.size())
            {
                return RETURN_NOT_FOUND;
            }

            // Check alpha
            if(isalpha(currentLine[startPos + variableLength]))
            {
                variableName += currentLine[startPos + variableLength];
                variableLength++;

                continue;
            }

            // If we already have an alpha we allow numeric 
            if(variableLength > 1)
            {
                if(isalpha(currentLine[startPos + variableLength]))
                {
                    variableName += currentLine[startPos + variableLength];
                    variableLength++;

                    continue;
                }
            }
            scanForEnd = false;
        }

        // If nothing was found, then nothing was found. Oh well
        if(variableLength == 0)
        {
            return RETURN_NOT_FOUND;
        }

        // Ensure the variable isn't a keyword
        if(matchesKeywordString(variableName))
        {
            return RETURN_NOT_FOUND;
        }

        // Setup the token!
        token.token = Token::VARIABLE;
        token.value = variableName;
        return (int)variableName.size() + startPos;
    }
}