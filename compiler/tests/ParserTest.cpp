#include <iostream>
#include "Parser.hpp"
#include "Tokens.hpp"
#include <vector>
#include "CppUTest/TestHarness.h"

namespace 
{
    struct TestCase
    {
        std::string given;
        std::string expected;
    };

    std::string concatedCallbackValues;

    void callback(std::vector<NABLA::TokenValue> tokens)
    {
        concatedCallbackValues = "";

        for(int i = 0; i < tokens.size(); i++)
        {
            concatedCallbackValues += tokens[i].value;
        }
    }

    bool receivedError;

    void errorCallback(NABLA::Parser::ErrorInformation ei)
    {
        receivedError = true;
    }
}

TEST_GROUP(NablaCompilerTest)
{   
};

#warning need to make tests unclude = and ==

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaCompilerTest, checkValid)
{

    NABLA::Parser parser(callback, errorCallback);

    std::vector<TestCase> testCases { 
        {"4.32", "4.32"},
        {"1 + 2 / 5 * ( 4 + ( 4 / 2 ) )", "1+2/5*(4+(4/2))"},
        {"4", "4"},
        {"5 /4", "5/4"},
        {"4 - 3", "4-3"},
        {"4 * 3 // With a comment", "4*3"},
        {"// Commented out line 4+3", ""},
        {"4 += 4", "4+=4"},  // This is silly, but I think its okay. Some might not think so
        {"4 /= 4", "4/=4"},  // This is silly, but I think its okay. Some might not think so
        {"4 -= 4", "4-=4"},  // This is silly, but I think its okay. Some might not think so
        {"4 *= 4", "4*=4"},  // This is silly, but I think its okay. Some might not think so
        {"4 == 4", "4==4"},
        {"(4 + 4)", "(4+4)"},
        {"(4 / 4)", "(4/4)"},
        {"(4 - 4)", "(4-4)"},
        {"(4 * 4)", "(4*4)"},
    };

    for(auto &i: testCases)
    {
        receivedError  = false;
        
        parser.scanLine(i.given);

        parser.indicateComplete();

        CHECK_EQUAL_TEXT(i.expected, concatedCallbackValues, "Values did not match expected");

        CHECK_FALSE(receivedError);

        // Reset the string 
        concatedCallbackValues = "";
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaCompilerTest, checkInvalid)
{
    NABLA::Parser parser(callback, errorCallback);

    std::vector<std::string> errors { 
        ".4", "45.", "77.44. ", "4.3 + 4 ( 8 - 4", 
        ") 4.5", ")(((()))))))))(", "5.6 )", "()",
        "4+", "+4", "+)4" 
    };

    for(auto &i: errors)
    {
        receivedError = false;
        
        parser.scanLine(i);

        parser.indicateComplete();

        CHECK_TRUE(receivedError);
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaCompilerTest, checkVarDecl)
{
    NABLA::Parser parser(callback, errorCallback);

    std::vector<TestCase> testCases { 
        {"int: myInt"  , "int:myInt"}, 
      //  {"str:myStr"  , "myStr"},
      //  {"real:myreal", "myreal"},
      //  {"int:  MyInt", "MyInt"}
    };

    for(auto &i: testCases)
    {
        receivedError  = false;
        
        parser.scanLine(i.given);

        parser.indicateComplete();

        CHECK_EQUAL_TEXT(i.expected, concatedCallbackValues, "Values did not match expected");

        CHECK_FALSE(receivedError);

        // Reset the string 
        concatedCallbackValues = "";
    }
}