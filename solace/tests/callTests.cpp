#include <iostream>
#include "bytegen.hpp"
#include "InsManifest.hpp"
#include <random>
#include "CppUTest/TestHarness.h"

namespace
{

    uint32_t getRandom32(uint32_t low, uint32_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        return dis(gen);
    }
}

TEST_GROUP(CallTests)
{   
    SOLACE::Bytegen byteGen;  
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(CallTests, AllCallTests)
{
    for(int16_t j = 0; j < 100; j++)
    {
        std::vector<SOLACE::Bytegen::Instruction> instructions;

        uint32_t destinationAddress = getRandom32(0, 500);
        uint32_t sourceAddress = getRandom32(0, 500);
        uint32_t sourceRegion = getRandom32(0, 500);


        SOLACE::Bytegen::Instruction cssf;
        cssf.bytes[0] = SOLACE::MANIFEST::INS_CS_SF;
        cssf.bytes[1] = (sourceAddress & 0xFF000000) >> 24;
        cssf.bytes[2] = (sourceAddress & 0x00FF0000) >> 16;
        cssf.bytes[3] = (sourceAddress & 0x0000FF00) >> 8 ;
        cssf.bytes[4] = (sourceAddress & 0x000000FF) >> 0 ;
        cssf.bytes[5] = 0xFF;
        cssf.bytes[6] = 0xFF;
        cssf.bytes[7] = 0xFF;

        instructions.push_back(cssf);

        SOLACE::Bytegen::Instruction cssr;
        cssr.bytes[0] = SOLACE::MANIFEST::INS_CS_SR;
        cssr.bytes[1] = (sourceRegion & 0xFF000000) >> 24;
        cssr.bytes[2] = (sourceRegion & 0x00FF0000) >> 16;
        cssr.bytes[3] = (sourceRegion & 0x0000FF00) >> 8 ;
        cssr.bytes[4] = (sourceRegion & 0x000000FF) >> 0 ;
        cssr.bytes[5] = 0xFF;
        cssr.bytes[6] = 0xFF;
        cssr.bytes[7] = 0xFF;

        instructions.push_back(cssr);

        SOLACE::Bytegen::Instruction call;

        call.bytes[0] = SOLACE::MANIFEST::INS_CALL;
        call.bytes[1] = (destinationAddress & 0xFF000000) >> 24;
        call.bytes[2] = (destinationAddress & 0x00FF0000) >> 16;
        call.bytes[3] = (destinationAddress & 0x0000FF00) >> 8 ;
        call.bytes[4] = (destinationAddress & 0x000000FF) >> 0 ;
        call.bytes[5] = 0xFF;
        call.bytes[6] = 0xFF;
        call.bytes[7] = 0xFF;

        instructions.push_back(call);

        std::vector<SOLACE::Bytegen::Instruction> generated = byteGen.createCallInstruction(
            sourceAddress,
            sourceRegion,
            destinationAddress
        );

        if(generated.size() != instructions.size())
        {
            FAIL("The number of instructions generated from createCallInstruction not as expected!");
        }

        for(int i = 0; i < generated.size(); i++)
        {
            // Build expected ins
            CHECK_EQUAL_TEXT(instructions[i].bytes[0], generated[i].bytes[0], "Opcode fail");
            CHECK_EQUAL_TEXT(instructions[i].bytes[1], generated[i].bytes[1], "Byte 1 fail");
            CHECK_EQUAL_TEXT(instructions[i].bytes[2], generated[i].bytes[2], "Byte 2 fail");
            CHECK_EQUAL_TEXT(instructions[i].bytes[3], generated[i].bytes[3], "Byte 3 fail");
            CHECK_EQUAL_TEXT(instructions[i].bytes[4], generated[i].bytes[4], "Byte 4 fail");
            CHECK_EQUAL_TEXT(instructions[i].bytes[5], generated[i].bytes[5], "Byte 5 fail");
            CHECK_EQUAL_TEXT(instructions[i].bytes[6], generated[i].bytes[6], "Byte 6 fail");
            CHECK_EQUAL_TEXT(instructions[i].bytes[7], generated[i].bytes[7], "Byte 7 fail");
        }
    }
}