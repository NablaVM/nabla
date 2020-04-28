#include <iostream>
#include "bytegen.hpp"
#include "VmInstructions.h"
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

    uint32_t getRandomLabelNumber()
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0x00, 0xFFFFFF);
        return dis(gen);
    }

    // Just in case the register addresses need to change, we don't want to have to change
    // the tests to accomodate
    uint8_t integerToRegister(int16_t reg)
    {
        switch(reg)
        {
            case 0 : return REGISTER_0 ;
            case 1 : return REGISTER_1 ;
            case 2 : return REGISTER_2 ;
            case 3 : return REGISTER_3 ;
            case 4 : return REGISTER_4 ;
            case 5 : return REGISTER_5 ;
            case 6 : return REGISTER_6 ;
            case 7 : return REGISTER_7 ;
            case 8 : return REGISTER_8 ;
            case 9 : return REGISTER_9 ;
            case 10: return REGISTER_10;
            case 11: return REGISTER_11;
            case 12: return REGISTER_12;
            case 13: return REGISTER_13;
            case 14: return REGISTER_14;
            case 15: return REGISTER_15;
            default: 
                std::cerr << "Someone tried something silly with : " << reg  << ". IN THE BRANCH TEST!" << std::endl;
                exit(EXIT_FAILURE); 
                break;
        }
    }
}

TEST_GROUP(BranchTests)
{   
    NABLA::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(BranchTests, AllBranchTests)
{
    for(int16_t j = 0; j < 1000; j++)
    {
        NABLA::Bytegen::Instruction expectedIns;
        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(getRandom8(1,12));

        uint8_t reg1   = getRandom8(0, 9);
        uint8_t reg2   = getRandom8(0, 9);
        uint32_t label = getRandomLabelNumber();

        switch(type)
        {
            case NABLA::Bytegen::BranchTypes::BGT  : expectedIns.bytes[0] = INS_BGT  ; break; 
            case NABLA::Bytegen::BranchTypes::BGTE : expectedIns.bytes[0] = INS_BGTE ; break; 
            case NABLA::Bytegen::BranchTypes::BLT  : expectedIns.bytes[0] = INS_BLT  ; break; 
            case NABLA::Bytegen::BranchTypes::BLTE : expectedIns.bytes[0] = INS_BLTE ; break; 
            case NABLA::Bytegen::BranchTypes::BEQ  : expectedIns.bytes[0] = INS_BEQ  ; break; 
            case NABLA::Bytegen::BranchTypes::BNE  : expectedIns.bytes[0] = INS_BNE  ; break; 
            case NABLA::Bytegen::BranchTypes::BGTD : expectedIns.bytes[0] = INS_BGTD ; break;
            case NABLA::Bytegen::BranchTypes::BGTED: expectedIns.bytes[0] = INS_BGTED; break;
            case NABLA::Bytegen::BranchTypes::BLTD : expectedIns.bytes[0] = INS_BLTD ; break;
            case NABLA::Bytegen::BranchTypes::BLTED: expectedIns.bytes[0] = INS_BLTED; break;
            case NABLA::Bytegen::BranchTypes::BEQD : expectedIns.bytes[0] = INS_BEQD ; break;
            case NABLA::Bytegen::BranchTypes::BNED : expectedIns.bytes[0] = INS_BNED ; break;
            default:  FAIL("Something bad happened"); break; // Keep that compiler happy.
        }
        expectedIns.bytes[1] = integerToRegister(reg1);
        expectedIns.bytes[2] = integerToRegister(reg2);
        expectedIns.bytes[3] = (label & 0xFF000000) >> 24 ;
        expectedIns.bytes[4] = (label & 0x00FF0000) >> 16 ;
        expectedIns.bytes[5] = (label & 0x0000FF00) >> 8  ;
        expectedIns.bytes[6] = (label & 0x000000FF) >> 0  ;
        expectedIns.bytes[7] = 0xFF;

        NABLA::Bytegen::Instruction ins = byteGen.createBranchInstruction(
            type,
            reg1,
            reg2,
            label
        );

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