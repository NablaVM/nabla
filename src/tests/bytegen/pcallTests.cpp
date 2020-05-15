#include <iostream>
#include "bytegen.hpp"
#include "VSysInstructions.hpp"
#include <random>
#include "CppUTest/TestHarness.h"

namespace
{
    uint32_t getRandom32(uint32_t low, uint32_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint32_t> dis(low, high);
        return dis(gen);
    }
}

TEST_GROUP(PCallTests)
{   
    NABLA::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(PCallTests, Pcall)
{
    for(int16_t j = 0; j < 50; j++)
    {
        NABLA::Bytegen::Instruction expectedIns;

        uint32_t address = getRandom32(0, std::numeric_limits<uint32_t>::max());

        expectedIns.bytes[0] = NABLA::VSYS::INS_PCALL;
        expectedIns.bytes[1] = (address & 0xFF000000) >> 24;
        expectedIns.bytes[2] = (address & 0x00FF0000) >> 16;
        expectedIns.bytes[3] = (address & 0x0000FF00) >> 8 ;
        expectedIns.bytes[4] = (address & 0x000000FF) >> 0 ;
        expectedIns.bytes[5] = 0xFF;
        expectedIns.bytes[6] = 0xFF;
        expectedIns.bytes[7] = 0xFF;
        
        NABLA::Bytegen::Instruction ins = byteGen.createPcallInstruction(address);

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
