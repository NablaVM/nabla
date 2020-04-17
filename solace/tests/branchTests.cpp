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
                std::cerr << "Someone tried something silly with : " << reg  << ". IN THE BRANCH TEST!" << std::endl;
                exit(EXIT_FAILURE); 
                break;
        }
    }
}

TEST_GROUP(BranchTests)
{   
    SOLACE::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(BranchTests, AllBranchTests)
{
    for(int16_t j = 0; j < 1000; j++)
    {
        SOLACE::Bytegen::Instruction expectedIns;
        SOLACE::Bytegen::BranchTypes type = static_cast<SOLACE::Bytegen::BranchTypes>(getRandom8(1,12));

        uint8_t reg1   = getRandom8(0, 15);
        uint8_t reg2   = getRandom8(0, 15);
        uint32_t label = getRandomLabelNumber();

        switch(type)
        {
            case SOLACE::Bytegen::BranchTypes::BGT  : expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BGT  ; break; 
            case SOLACE::Bytegen::BranchTypes::BGTE : expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BGTE ; break; 
            case SOLACE::Bytegen::BranchTypes::BLT  : expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BLT  ; break; 
            case SOLACE::Bytegen::BranchTypes::BLTE : expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BLTE ; break; 
            case SOLACE::Bytegen::BranchTypes::BEQ  : expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BEQ  ; break; 
            case SOLACE::Bytegen::BranchTypes::BNE  : expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BNE  ; break; 
            case SOLACE::Bytegen::BranchTypes::BGTD : expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BGTD ; break;
            case SOLACE::Bytegen::BranchTypes::BGTED: expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BGTED; break;
            case SOLACE::Bytegen::BranchTypes::BLTD : expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BLTD ; break;
            case SOLACE::Bytegen::BranchTypes::BLTED: expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BLTED; break;
            case SOLACE::Bytegen::BranchTypes::BEQD : expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BEQD ; break;
            case SOLACE::Bytegen::BranchTypes::BNED : expectedIns.bytes[0] = SOLACE::MANIFEST::INS_BNED ; break;
            default:  FAIL("Something bad happened"); break; // Keep that compiler happy.
        }
        expectedIns.bytes[1] = integerToRegister(reg1);
        expectedIns.bytes[2] = integerToRegister(reg2);
        expectedIns.bytes[3] = (label & 0xFF000000) >> 24 ;
        expectedIns.bytes[4] = (label & 0x00FF0000) >> 16 ;
        expectedIns.bytes[5] = (label & 0x0000FF00) >> 8  ;
        expectedIns.bytes[6] = (label & 0x000000FF) >> 0  ;
        expectedIns.bytes[7] = 0xFF;

        SOLACE::Bytegen::Instruction ins = byteGen.createBranchInstruction(
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