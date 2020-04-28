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

TEST_GROUP(MovTests)
{   
    NABLA::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(MovTests, AllMovTests)
{
    for(int16_t j = 0; j < 100; j++)
    {
        NABLA::Bytegen::Instruction expectedIns;

        NABLA::Bytegen::MovSetup setup = (j % 2 == 0 ) ? NABLA::Bytegen::MovSetup::REG_REG :
                                                         NABLA::Bytegen::MovSetup::REG_NUM;
        
        if(setup == NABLA::Bytegen::MovSetup::REG_REG)
        {
            expectedIns.bytes[0] = INS_MOV;
            expectedIns.bytes[1] = integerToRegister(getRandom8(0, 9)) ;
            expectedIns.bytes[2] = integerToRegister(getRandom8(0, 9)) ;
            expectedIns.bytes[3] = 0xFF;
            expectedIns.bytes[4] = 0xFF;
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;
        }
        else
        {
            expectedIns.bytes[0] = (INS_MOV | 0x01);
            expectedIns.bytes[1] = integerToRegister(getRandom8(0, 9)) ;
            expectedIns.bytes[2] = getRandom8(0, 9);
            expectedIns.bytes[3] = 0xFF;
            expectedIns.bytes[4] = 0xFF;
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;
        }
        
        
        NABLA::Bytegen::Instruction ins = byteGen.createMovInstruction(
            setup,
            expectedIns.bytes[1],
            expectedIns.bytes[2]
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