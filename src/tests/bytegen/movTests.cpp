#include <iostream>
#include "bytegen.hpp"
#include "VSysInstructions.hpp"
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
            case 0 : return NABLA::VSYS::REGISTER_0 ;
            case 1 : return NABLA::VSYS::REGISTER_1 ;
            case 2 : return NABLA::VSYS::REGISTER_2 ;
            case 3 : return NABLA::VSYS::REGISTER_3 ;
            case 4 : return NABLA::VSYS::REGISTER_4 ;
            case 5 : return NABLA::VSYS::REGISTER_5 ;
            case 6 : return NABLA::VSYS::REGISTER_6 ;
            case 7 : return NABLA::VSYS::REGISTER_7 ;
            case 8 : return NABLA::VSYS::REGISTER_8 ;
            case 9 : return NABLA::VSYS::REGISTER_9 ;
            case 10: return NABLA::VSYS::REGISTER_10;
            case 11: return NABLA::VSYS::REGISTER_11;
            case 12: return NABLA::VSYS::REGISTER_12;
            case 13: return NABLA::VSYS::REGISTER_13;
            case 14: return NABLA::VSYS::REGISTER_14;
            case 15: return NABLA::VSYS::REGISTER_15;
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
        
        uint8_t  arg_one;
        uint32_t arg_two;

        if(setup == NABLA::Bytegen::MovSetup::REG_REG)
        {
            expectedIns.bytes[0] = NABLA::VSYS::INS_MOV;
            expectedIns.bytes[1] = integerToRegister(getRandom8(0, 9)) ;
            expectedIns.bytes[2] = integerToRegister(getRandom8(0, 9)) ;
            expectedIns.bytes[3] = 0xFF;
            expectedIns.bytes[4] = 0xFF;
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;

            arg_one = expectedIns.bytes[1];
            arg_two = expectedIns.bytes[2];
        }
        else
        {
            uint32_t randomNum = getRandom32(0, 65500);
            expectedIns.bytes[0] = (NABLA::VSYS::INS_MOV | 0x01);
            expectedIns.bytes[1] = integerToRegister(getRandom8(0, 9)) ;
            expectedIns.bytes[2] = (randomNum & 0xFF000000) >> 24 ;
            expectedIns.bytes[3] = (randomNum & 0x00FF0000) >> 16 ;
            expectedIns.bytes[4] = (randomNum & 0x0000FF00) >> 8  ;
            expectedIns.bytes[5] = (randomNum & 0x000000FF) >> 0  ;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;
            
            arg_one = expectedIns.bytes[1];
            arg_two = randomNum;
        }
        
        NABLA::Bytegen::Instruction ins = byteGen.createMovInstruction(
            setup,
            arg_one,
            arg_two
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