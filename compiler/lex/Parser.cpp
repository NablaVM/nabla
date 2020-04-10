#include "Parser.hpp"

#include <iostream>
#include <ctype.h>
#include <string>

namespace NABLA
{


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

        for(int i = 0; i < currentLine.length(); i++)
        {
            switch (currentLine[i])
            {
            case ' ' : break;
            case '\t': break;
            case '\r': break;

            // -------------------------------------------------------------------------------------
            //  Statement end
            // -------------------------------------------------------------------------------------
            case ';': 
                // Send tokens off and clear the vector
                indicateComplete();
                break;

            // -------------------------------------------------------------------------------------
            //  Open Paren
            // -------------------------------------------------------------------------------------
            case '(': 
            {
                bool closeFound = false;
                for(int c = i; c < currentLine.size() && !closeFound; c++)
                {
                    if(currentLine[c] == ')') { closeFound = true; }
                }

                if(closeFound)
                {
                    tokenValues.push_back(TokenValue{ Token::L_PAREN, "(" }); 
                }
                else 
                {
                    errorCallback(createError("Syntax Error. No matching ')' found."));
                    return false;
                }

                break;
            }

            // -------------------------------------------------------------------------------------
            //  Closed Paren
            // -------------------------------------------------------------------------------------
            case ')': 
            {
                bool openFound = false;
                for(int c = i; c >= 0 && !openFound; c--)
                {
                    if(currentLine[c] == '(') { openFound = true; }
                }

                if(openFound)
                {
                    tokenValues.push_back(TokenValue{ Token::R_PAREN, ")" });
                }
                else 
                {
                    errorCallback(createError("Syntax Error. No matching '(' found."));
                    return false;
                }

                break;
            }

            // -------------------------------------------------------------------------------------
            //  +
            // -------------------------------------------------------------------------------------
            case '+': tokenValues.push_back(TokenValue{ Token::OP_ADD,  "+" }); break;

            // -------------------------------------------------------------------------------------
            //  -
            // -------------------------------------------------------------------------------------
            case '-': tokenValues.push_back(TokenValue{ Token::OP_MUL,  "-" }); break;

            // -------------------------------------------------------------------------------------
            //  *
            // -------------------------------------------------------------------------------------
            case '*': tokenValues.push_back(TokenValue{ Token::OP_SUB,  "*" }); break;

            // -------------------------------------------------------------------------------------
            //  / Could be a comment, could be a div could be a diveq
            // -------------------------------------------------------------------------------------
            case '/':
            {
                if(matchNext('/', i+1))
                {
                    // A comment was found. Ignore the lines
                    return true;
                }
                else
                {
                    tokenValues.push_back(TokenValue{ Token::OP_DIV,  "/" }); 
                }
                break;
            }
            
            // -------------------------------------------------------------------------------------
            //  Everything else
            // -------------------------------------------------------------------------------------
            default:

                // ----------------------------------------------------------------------------------
                //  Locate a number ( 42 | 42.22 )
                // ----------------------------------------------------------------------------------
                if(isdigit(currentLine[i]))
                {
                    // Assume it is an integer until a '.' is found
                    bool isReal = false;

                    // Accumulate the numbers
                    std::string digits; digits += currentLine[i];

                    // Scan up-to the rest of the line for the remaining parts of the number
                    for(int c = i+1; c < currentLine.size(); c++)
                    {
                        // If its a digit, add it 
                        if(isdigit(currentLine[c]))
                        {
                            digits += currentLine[c];
                            i++;
                        }

                        // If its a '.' it can be assumed that its a 'real'
                        else if(currentLine[c] == '.')
                        {
                            isReal = true;

                            // Ensure that the next item is a digit
                            if(c + 1 == currentLine.size() || !isdigit(currentLine[c+1]) )
                            {
                                errorCallback(createError("Syntax error. Expected a number following : " + digits + ". "));
                                return false;
                            }

                            digits += currentLine[c];
                            i++;
                        }
                        else
                        {
                            c = currentLine.size(); // Kill the loop
                        }
                    } // End inner search


                    if (isReal)
                    {
                        tokenValues.push_back(TokenValue{Token::REAL, digits});
                    }
                    else 
                    {
                        tokenValues.push_back(TokenValue{Token::INTEGER, digits});
                    }

                    break;

                } 

                // ----------------------------------------------------------------------------------
                //  Locate a literal
                // ----------------------------------------------------------------------------------



                // ----------------------------------------------------------------------------------
                //  Everything else should be an error
                // ----------------------------------------------------------------------------------

                std::string e = "Error. Unexpected value: [";  e += currentLine[i]; e += "] found";
                errorCallback(createError(e));
                return false;
                break;
            }


        }

        return true;
    }

}