#include <iostream>
#include "bytegen.hpp"
#include "VSysInstructions.hpp"
#include <random>
#include "CppUTest/TestHarness.h"

TEST_GROUP(NopTests)
{   
    NABLA::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NopTests, AllNopTests)
{
    for(int16_t j = 0; j < 100; j++)
    {
        NABLA::Bytegen::Instruction expectedIns;

        expectedIns.bytes[0] = NABLA::VSYS::INS_NOP;
        expectedIns.bytes[1] = 0xFF;
        expectedIns.bytes[2] = 0xFF;
        expectedIns.bytes[3] = 0xFF;
        expectedIns.bytes[4] = 0xFF;
        expectedIns.bytes[5] = 0xFF;
        expectedIns.bytes[6] = 0xFF;
        expectedIns.bytes[7] = 0xFF;

        NABLA::Bytegen::Instruction ins = byteGen.createNopInstruction();

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