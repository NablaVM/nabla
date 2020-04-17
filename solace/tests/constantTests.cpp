#include <iostream>
#include "bytegen.hpp"
#include "InsManifest.hpp"
#include <random>
#include<ieee754.h>
#include "CppUTest/TestHarness.h"

namespace
{
    uint16_t getRandom16(uint16_t low, uint16_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint16_t> dis(low, high);
        return dis(gen);
    }

    uint32_t getRandom32(uint32_t low, uint32_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(low, high);
        return dis(gen);
    }

    uint64_t getRandom64(uint64_t low, uint64_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(low, high);
        return dis(gen);
    }
    
    double getRandomFp(double low, double high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(low, high);
        return dis(gen);
    }
}

TEST_GROUP(ConstantTests)
{   
    SOLACE::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ConstantTests, integers)
{
    // .int8
    for(uint8_t j = 0; j < 255; j++)
    {
        uint8_t val = j;

        std::vector<uint8_t> expectedBytes;

        expectedBytes.push_back( SOLACE::MANIFEST::CONST_INT  |  0x0 );
        expectedBytes.push_back( ( val & 0x000000FF) >> 0  );

        std::vector<uint8_t> actual = byteGen.createConstantInt(val, SOLACE::Bytegen::Integers::EIGHT);

        if(expectedBytes.size() != actual.size()) { FAIL(".int8 construction byte length mismatch"); }

        for(int i = 0; i < expectedBytes.size(); i++)
        {
            CHECK_EQUAL_TEXT(expectedBytes[i], actual[i], "Mistmatched byte in .int8");
        }
    }

    // .int16
    for(uint8_t j = 0; j < 50; j++)
    {
        uint16_t val = getRandom16(0, 65535);

        std::vector<uint8_t> expectedBytes;

        expectedBytes.push_back( SOLACE::MANIFEST::CONST_INT  |  0x1 );
        expectedBytes.push_back( (val & 0x0000FF00) >> 8  );
        expectedBytes.push_back( (val & 0x000000FF) >> 0  );

        std::vector<uint8_t> actual = byteGen.createConstantInt(val, SOLACE::Bytegen::Integers::SIXTEEN);

        if(expectedBytes.size() != actual.size()) { FAIL(".int16 construction byte length mismatch"); }

        for(int i = 0; i < expectedBytes.size(); i++)
        {
            CHECK_EQUAL_TEXT(expectedBytes[i], actual[i], "Mistmatched byte in .int16");
        }
    }
    
    // .int32
    for(uint8_t j = 0; j < 50; j++)
    {
        uint32_t val = getRandom32(0, 0x7FFFFFF);

        std::vector<uint8_t> expectedBytes;

        expectedBytes.push_back( SOLACE::MANIFEST::CONST_INT  |  0x2 );
        expectedBytes.push_back( (val & 0xFF000000) >> 24 );
        expectedBytes.push_back( (val & 0x00FF0000) >> 16 );
        expectedBytes.push_back( (val & 0x0000FF00) >> 8  );
        expectedBytes.push_back( (val & 0x000000FF) >> 0  );

        std::vector<uint8_t> actual = byteGen.createConstantInt(val, SOLACE::Bytegen::Integers::THIRTY_TWO);

        if(expectedBytes.size() != actual.size()) { FAIL(".int32 construction byte length mismatch"); }

        for(int i = 0; i < expectedBytes.size(); i++)
        {
            CHECK_EQUAL_TEXT(expectedBytes[i], actual[i], "Mistmatched byte in .int32");
        }
    }

    // .int64
    for(uint8_t j = 0; j < 50; j++)
    {
        uint64_t val = getRandom32(0, 0xFFFFFFFF);

        std::vector<uint8_t> expectedBytes;

        expectedBytes.push_back( SOLACE::MANIFEST::CONST_INT  |  0x3 );
        expectedBytes.push_back( (val & 0xFF00000000000000) >> 56 );
        expectedBytes.push_back( (val & 0x00FF000000000000) >> 48 );
        expectedBytes.push_back( (val & 0x0000FF0000000000) >> 40 );
        expectedBytes.push_back( (val & 0x000000FF00000000) >> 32 );
        expectedBytes.push_back( (val & 0x00000000FF000000) >> 24 );
        expectedBytes.push_back( (val & 0x0000000000FF0000) >> 16 );
        expectedBytes.push_back( (val & 0x000000000000FF00) >> 8  );
        expectedBytes.push_back( (val & 0x00000000000000FF) >> 0  );

        std::vector<uint8_t> actual = byteGen.createConstantInt(val, SOLACE::Bytegen::Integers::SIXTY_FOUR);

        if(expectedBytes.size() != actual.size()) { FAIL(".int64 construction byte length mismatch"); }

        for(int i = 0; i < expectedBytes.size(); i++)
        {
            CHECK_EQUAL_TEXT(expectedBytes[i], actual[i], "Mistmatched byte in .int64");
        }
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ConstantTests, doublePrecisionFp)
{
    for(int j = 0; j < 50; j++)
    {
        double dval = getRandomFp(-0.0000, 10000.0);

        std::vector<uint8_t> expected;

        ieee754_double ied;

        ied.d = dval;

        uint64_t packed = ied.ieee.negative  | 
                            ied.ieee.exponent  |
                            ied.ieee.mantissa0 |
                            ied.ieee.mantissa1;

        expected.push_back( SOLACE::MANIFEST::CONST_DBL      );
        expected.push_back( (packed & 0xFF00000000000000) >> 56 );
        expected.push_back( (packed & 0x00FF000000000000) >> 48 );
        expected.push_back( (packed & 0x0000FF0000000000) >> 40 );
        expected.push_back( (packed & 0x000000FF00000000) >> 32 );
        expected.push_back( (packed & 0x00000000FF000000) >> 24 );
        expected.push_back( (packed & 0x0000000000FF0000) >> 16 );
        expected.push_back( (packed & 0x000000000000FF00) >> 8  );
        expected.push_back( (packed & 0x00000000000000FF) >> 0  );

        std::vector<uint8_t> actual = byteGen.createConstantDouble(dval);

        if(expected.size() != actual.size()) { FAIL(".double construction byte length mismatch"); }

        for(int i = 0; i < expected.size(); i++)
        {
            CHECK_EQUAL_TEXT(expected[i], actual[i], "Mistmatched byte in .double");
        }
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ConstantTests, leStringies)
{
    // Commentary, otter facts, and some google news headlines
    std::vector<std::string> testStrings = {
        "This is a test of the constant strings!",
        "I really hope that constant strings will even be useful seeing as they are capped at 255",
        "Maybe they will be removed one day, but for now they stay",
        "Who knows, certainly not me!",
        "Thirteen different species of otter exist all around the world. Some are small river otters, and some are bigger sea otters.",
        "90% of all sea otters live on the coast of Alaska. ",
        "They're hungry animals! Sea otters eat 25% of their body weight in food every day.",
        "Unlike most marine mammals, they don't have a layer of blubber. But, they do have the thickest fur of all animals.",
        "Samsung’s midrange Galaxy S10 Lite comes to the US for $650",
        "The New Nintendo Switch May Feature Dual Screens",
        "The New iPad Pro Inches Closer to Becoming the Laptop I Want -- wait what?",
    };

    for(auto &s : testStrings)
    {
        std::vector<uint8_t> expected;
        
        expected.push_back( SOLACE::MANIFEST::CONST_STR );

        // If the string size is greater than max, then we will lop-off any extra.
        uint8_t strSize = ( (s.size() > 255) ? 255 : s.size() );

        // Add the size
        expected.push_back(strSize);

        // Ensure we only place the values we accounted for
        for(uint8_t i = 0; i < strSize; i++)
        {
            expected.push_back(static_cast<uint8_t>(s[i]));
        }
        
        std::vector<uint8_t> actual = byteGen.createConstantString(s);

        if(expected.size() != actual.size()) { FAIL(".string construction byte length mismatch"); }

        for(int i = 0; i < expected.size(); i++)
        {
            CHECK_EQUAL_TEXT(expected[i], actual[i], "Mistmatched byte in .string");
        }
    }
}