#include <iostream>
#include "bytegen.hpp"
#include "InsManifest.hpp"
#include <random>
#include "CppUTest/TestHarness.h"

namespace
{

    uint8_t getRandom8(uint8_t low, uint8_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        return dis(gen);
    }

    uint32_t getRandom32(uint32_t low, uint32_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        return dis(gen);
    }

    // Just in case the register addresses need to change, we don't want to have to change
    // the tests to accomodate
    uint8_t integerToRegister(int16_t reg)
    {
        switch(reg)
        {
            case 0 : return SOLACE::MANIFEST::REGISTER_0 ;
            case 1 : return SOLACE::MANIFEST::REGISTER_1 ;
            case 2 : return SOLACE::MANIFEST::REGISTER_2 ;
            case 3 : return SOLACE::MANIFEST::REGISTER_3 ;
            case 4 : return SOLACE::MANIFEST::REGISTER_4 ;
            case 5 : return SOLACE::MANIFEST::REGISTER_5 ;
            case 6 : return SOLACE::MANIFEST::REGISTER_6 ;
            case 7 : return SOLACE::MANIFEST::REGISTER_7 ;
            case 8 : return SOLACE::MANIFEST::REGISTER_8 ;
            case 9 : return SOLACE::MANIFEST::REGISTER_9 ;
            case 10: return SOLACE::MANIFEST::REGISTER_10;
            case 11: return SOLACE::MANIFEST::REGISTER_11;
            case 12: return SOLACE::MANIFEST::REGISTER_12;
            case 13: return SOLACE::MANIFEST::REGISTER_13;
            case 14: return SOLACE::MANIFEST::REGISTER_14;
            case 15: return SOLACE::MANIFEST::REGISTER_15;
            default: 
                std::cerr << "Someone tried something silly with : " << reg  << ". IN THE STB TEST!" << std::endl;
                exit(EXIT_FAILURE); 
                break;
        }
    }

    // Get a stack address given the stack type
    uint8_t getStackAddress(SOLACE::Bytegen::Stacks stack)
    {
        switch(stack)
        {
            case SOLACE::Bytegen::Stacks::GLOBAL:
                return SOLACE::MANIFEST::GLOBAL_STACK;
            case SOLACE::Bytegen::Stacks::LOCAL:
                return SOLACE::MANIFEST::LOCAL_STACK;
            default:
                std::cerr << "Someone tried something silly awful with getStackAddress in StbLdbTests" << std::endl;
                exit(EXIT_FAILURE); 
                return 0;
        }
        return 0; // Keep the compiler happy
    }
}

TEST_GROUP(StbLdbTests)
{   
    SOLACE::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(StbLdbTests, Stb)
{
    for(int16_t j = 0; j < 50; j++)
    {
        SOLACE::Bytegen::Instruction expectedIns;

        SOLACE::Bytegen::Stacks stackType = (j % 2 == 0) ? SOLACE::Bytegen::Stacks::GLOBAL : SOLACE::Bytegen::Stacks::LOCAL;

        uint32_t location = getRandom32(0, 0xFFFFFFF);

        expectedIns.bytes[0] = SOLACE::MANIFEST::INS_STB;
        expectedIns.bytes[1] = getStackAddress(stackType);
        expectedIns.bytes[2] = (location & 0xFF000000) >> 24 ;
        expectedIns.bytes[3] = (location & 0x00FF0000) >> 16 ;
        expectedIns.bytes[4] = (location & 0x0000FF00) >> 8  ;
        expectedIns.bytes[5] = (location & 0x000000FF) >> 0  ;
        expectedIns.bytes[6] = integerToRegister(getRandom8(0, 15));
        expectedIns.bytes[7] = 0xFF;
        
        SOLACE::Bytegen::Instruction ins = byteGen.createStbInstruction(
            stackType,
            location,
            expectedIns.bytes[6]
        );

        // Build expected ins
        CHECK_EQUAL_TEXT(ins.bytes[0], expectedIns.bytes[0], "Opcode fail");
        CHECK_EQUAL_TEXT(ins.bytes[1], expectedIns.bytes[1], "Incorrect Stack");
        CHECK_EQUAL_TEXT(ins.bytes[2], expectedIns.bytes[2], "Byte 1 fail");
        CHECK_EQUAL_TEXT(ins.bytes[3], expectedIns.bytes[3], "Byte 3 fail");
        CHECK_EQUAL_TEXT(ins.bytes[4], expectedIns.bytes[4], "Byte 4 fail");
        CHECK_EQUAL_TEXT(ins.bytes[5], expectedIns.bytes[5], "Byte 5 fail");
        CHECK_EQUAL_TEXT(ins.bytes[6], expectedIns.bytes[6], "Register fail");
        CHECK_EQUAL_TEXT(ins.bytes[7], expectedIns.bytes[7], "Byte 7 fail");
    }

    
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(StbLdbTests, Ldb)
{
    for(int16_t j = 0; j < 50; j++)
    {
        SOLACE::Bytegen::Instruction expectedIns;

        SOLACE::Bytegen::Stacks stackType = (j % 2 == 0) ? SOLACE::Bytegen::Stacks::GLOBAL : SOLACE::Bytegen::Stacks::LOCAL;

        uint32_t location = getRandom32(0, 0xFFFFFFF);

        expectedIns.bytes[0] = SOLACE::MANIFEST::INS_LDB;
        expectedIns.bytes[1] = integerToRegister(getRandom8(0, 15));
        expectedIns.bytes[2] = getStackAddress(stackType);
        expectedIns.bytes[3] = (location & 0xFF000000) >> 24 ;
        expectedIns.bytes[4] = (location & 0x00FF0000) >> 16 ;
        expectedIns.bytes[5] = (location & 0x0000FF00) >> 8  ;
        expectedIns.bytes[6] = (location & 0x000000FF) >> 0  ;
        expectedIns.bytes[7] = 0xFF;
        
        SOLACE::Bytegen::Instruction ins = byteGen.createLdbInstruction(
            stackType,
            location,
            expectedIns.bytes[1]
        );

        // Build expected ins
        CHECK_EQUAL_TEXT(ins.bytes[0], expectedIns.bytes[0], "Opcode fail");
        CHECK_EQUAL_TEXT(ins.bytes[1], expectedIns.bytes[1], "Incorrect Register");
        CHECK_EQUAL_TEXT(ins.bytes[2], expectedIns.bytes[2], "Incorrect stack");
        CHECK_EQUAL_TEXT(ins.bytes[3], expectedIns.bytes[3], "Byte 3 fail");
        CHECK_EQUAL_TEXT(ins.bytes[4], expectedIns.bytes[4], "Byte 4 fail");
        CHECK_EQUAL_TEXT(ins.bytes[5], expectedIns.bytes[5], "Byte 5 fail");
        CHECK_EQUAL_TEXT(ins.bytes[6], expectedIns.bytes[6], "Byte 6 fail");
        CHECK_EQUAL_TEXT(ins.bytes[7], expectedIns.bytes[7], "Byte 7 fail");
    }

    
}