#include <iostream>
#include "bytegen.hpp"
#include "InsManifest.hpp"
#include <random>
#include "CppUTest/TestHarness.h"

namespace
{
    uint32_t getRandomLabelNumber()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0x00, 0xFFFFFF);
        return dis(gen);
    }
}

TEST_GROUP(JumpTests)
{   
    SOLACE::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(JumpTests, AllJumpTests)
{
    for(int16_t j = 0; j < 1000; j++)
    {
        SOLACE::Bytegen::Instruction expectedIns;

        uint32_t location = getRandomLabelNumber();

        expectedIns.bytes[0] = SOLACE::MANIFEST::INS_JUMP;
        expectedIns.bytes[1] = (location & 0xFF000000) >> 24 ;
        expectedIns.bytes[2] = (location & 0x00FF0000) >> 16 ;
        expectedIns.bytes[3] = (location & 0x0000FF00) >> 8  ;
        expectedIns.bytes[4] = (location & 0x000000FF) >> 0  ;
        expectedIns.bytes[5] = 0xFF;
        expectedIns.bytes[6] = 0xFF;
        expectedIns.bytes[7] = 0xFF;

        SOLACE::Bytegen::Instruction ins = byteGen.createJumpInstruction(location);

        // Build expected ins
        CHECK_EQUAL_TEXT(ins.bytes[0], expectedIns.bytes[0], "Opcode fail");
        CHECK_EQUAL_TEXT(ins.bytes[1], expectedIns.bytes[1], "Byte 1 fail");
        CHECK_EQUAL_TEXT(ins.bytes[2], expectedIns.bytes[2], "Byte 2 fail");
        CHECK_EQUAL_TEXT(ins.bytes[3], expectedIns.bytes[3], "Byte 3 fail");
        CHECK_EQUAL_TEXT(ins.bytes[4], expectedIns.bytes[4], "Byte 4 fail");
        CHECK_EQUAL_TEXT(ins.bytes[5], expectedIns.bytes[5], "Byte 5 fail");
        CHECK_EQUAL_TEXT(ins.bytes[6], expectedIns.bytes[6], "Byte 6 fail");
        CHECK_EQUAL_TEXT(ins.bytes[7], expectedIns.bytes[7], "Byte 7 fail");
    }
}