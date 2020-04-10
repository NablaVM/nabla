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
    std::vector<NABLA::Token> *recvTokens;

    void callback(std::vector<NABLA::TokenValue> tokens)
    {
        concatedCallbackValues = "";

        for(int i = 0; i < tokens.size(); i++)
        {
            concatedCallbackValues += tokens[i].value;

            if(nullptr != recvTokens)
            {
                recvTokens->push_back(tokens[i].token);
            }
        }
    }

    bool receivedError;

    void errorCallback(NABLA::Parser::ErrorInformation ei)
    {
        std::cout << ei.errorMessage << std::endl;
        receivedError = true;
    }
}

TEST_GROUP(NablaCompilerTest)
{   
};

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
        {"myVar7 = 4", "myVar7=4"},
        {"myvar==4", "myvar==4"},
        {"4 == mf", "4==mf"},
        {"functionCall()", "functionCall()"},
        {"functionCall1()", "functionCall1()"}
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

TEST(NablaCompilerTest, checkPrimVarDecl)
{
    NABLA::Parser parser(callback, errorCallback);

    struct DeclCheck
    {
        std::string input;
        std::string expectedOutput;
        std::vector<NABLA::Token> expectedTokens;   // Expected tokens
        std::vector<NABLA::Token> actualTokens;     // These will be populated by the callback
    };

    std::vector<DeclCheck> testCases = {
        DeclCheck{ "int: myInteger", "int:myInteger", {NABLA::Token::INT_DECL , NABLA::Token::VARIABLE}, {}},
        DeclCheck{ "str: myString" , "str:myString" , {NABLA::Token::STR_DECL , NABLA::Token::VARIABLE}, {}},
        DeclCheck{ "real: soRealRn", "real:soRealRn", {NABLA::Token::REAL_DECL, NABLA::Token::VARIABLE}, {}},
        DeclCheck{ "int:myInteger", "int:myInteger" , {NABLA::Token::INT_DECL , NABLA::Token::VARIABLE}, {}},
        DeclCheck{ "str:myString" , "str:myString"  , {NABLA::Token::STR_DECL , NABLA::Token::VARIABLE}, {}},
        DeclCheck{ "real:soRealRn", "real:soRealRn" , {NABLA::Token::REAL_DECL, NABLA::Token::VARIABLE}, {}},
    };

    for(auto &i: testCases)
    {
        receivedError  = false;
        
        recvTokens = &i.actualTokens;

        parser.scanLine(i.input);

        parser.indicateComplete();

        CHECK_EQUAL_TEXT(i.expectedOutput, concatedCallbackValues, "Values did not match expected");

        CHECK_FALSE(receivedError);

        for(int j = 0; j < i.expectedTokens.size(); j++)
        {
            CHECK_TRUE_TEXT((i.expectedTokens.at(j) ==  recvTokens->at(j)), "Expected token != received token");
        }

        // Reset the string 
        concatedCallbackValues = "";

        recvTokens = nullptr;
    }
}