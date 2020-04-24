#include <iostream>
#include "bytegen.hpp"
#include "VmInstructions.h"
#include <random>
#include "CppUTest/TestHarness.h"

namespace
{
    uint8_t addOppCode(uint8_t ins, NABLA::Bytegen::ArithmaticSetup setup)
    {
        if(setup == NABLA::Bytegen::ArithmaticSetup::REG_REG){ return (ins | 0x00); }
        if(setup == NABLA::Bytegen::ArithmaticSetup::REG_NUM){ return (ins | 0x01); }
        if(setup == NABLA::Bytegen::ArithmaticSetup::NUM_REG){ return (ins | 0x02); }
        if(setup == NABLA::Bytegen::ArithmaticSetup::NUM_NUM){ return (ins | 0x03); }
        return ins;
    }

    uint16_t getRandom16(uint16_t low, uint16_t high)
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
                std::cerr << "Someone tried something silly with : " << reg  << ". IN THE ARITH TEST!" << std::endl;
                exit(EXIT_FAILURE); 
                break;
        }
    }
}

TEST_GROUP(BitwiseTests)
{   
    NABLA::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(BitwiseTests, AllBitwiseTests)
{
    int setupMod = 0;

    for(int16_t j = 0; j < 1000; j++)
    {
        setupMod++;
        int16_t destReg = integerToRegister(getRandom16(0,15));

        int16_t arg1;
        int16_t arg2;

        NABLA::Bytegen::BitwiseTypes type  =  static_cast<NABLA::Bytegen::BitwiseTypes>(getRandom16(1,6));
        NABLA::Bytegen::ArithmaticSetup setup;
        NABLA::Bytegen::Instruction expectedIns;

        // Since NOT is included, in bitwise but has a WHOLE WERID definition, 
        // we leverage random to change-up the means of test generation
        if(type == NABLA::Bytegen::BitwiseTypes::NOT)
        {
            setupMod = (getRandom16(0,1)) ? 2 : 4;
        }
        else
        {
            setupMod = (getRandom16(1,4));
        }

        if(setupMod == 1)
        { 
            setup = NABLA::Bytegen::ArithmaticSetup::NUM_NUM; 
            arg1 = getRandom16(0, 60000);   // NUM
            arg2 = getRandom16(0, 60000);   // NUM

            expectedIns.bytes[1] = integerToRegister(destReg);
            uint16_t num = static_cast<uint16_t>(arg1);

            expectedIns.bytes[2] = ( (num & 0xFF00) >> 8 ) ;
            expectedIns.bytes[3] = ( (num & 0x00FF) >> 0 ) ;

            uint16_t num1 = static_cast<uint16_t>(arg2);

            expectedIns.bytes[4] = ( (num1 & 0xFF00) >> 8 ) ;
            expectedIns.bytes[5] = ( (num1 & 0x00FF) >> 0 ) ;

            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;
        }

        if(setupMod == 2)
        {
            setup = NABLA::Bytegen::ArithmaticSetup::NUM_REG; 
            arg1 = getRandom16(0, 60000);   // NUM
            arg2 = getRandom16(0, 15);      // REG

            expectedIns.bytes[1] = integerToRegister(destReg);

            uint16_t num = static_cast<uint16_t>(arg1);
            expectedIns.bytes[2] = ( (num & 0xFF00) >> 8 ) ;
            expectedIns.bytes[3] = ( (num & 0x00FF) >> 0 ) ;
            expectedIns.bytes[4] = integerToRegister(arg2);
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;

            if(type == NABLA::Bytegen::BitwiseTypes::NOT)
            {
                expectedIns.bytes[4] = 0xFF;
            }
        }

        if(setupMod == 3)
        {
            setup = NABLA::Bytegen::ArithmaticSetup::REG_NUM; 
            arg1 = getRandom16(0, 15);      // REG
            arg2 = getRandom16(0, 60000);   // NUM

            expectedIns.bytes[1] = integerToRegister(destReg);
            expectedIns.bytes[2] = integerToRegister(arg1);
            uint16_t num = static_cast<uint16_t>(arg2);
            expectedIns.bytes[3] = ( (num & 0xFF00) >> 8 ) ;
            expectedIns.bytes[4] = ( (num & 0x00FF) >> 0 ) ;
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;
        }

        if(setupMod == 4)
        {
            setup = NABLA::Bytegen::ArithmaticSetup::REG_REG; 
            arg1 = getRandom16(0, 15);      // REG
            arg2 = getRandom16(0, 15);      // REG

            expectedIns.bytes[1] = integerToRegister(destReg);
            expectedIns.bytes[2] = integerToRegister(arg1);
            expectedIns.bytes[3] = integerToRegister(arg2);
            expectedIns.bytes[4] = 0xFF;
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;

            if(type == NABLA::Bytegen::BitwiseTypes::NOT)
            {
                expectedIns.bytes[3] = 0xFF;
            }
        }


        // Set the expected op + ins
        switch(type)
        {
            case NABLA::Bytegen::BitwiseTypes::LSH: expectedIns.bytes[0] = addOppCode(INS_LSH, setup); break;
            case NABLA::Bytegen::BitwiseTypes::RSH: expectedIns.bytes[0] = addOppCode(INS_RSH, setup); break; 
            case NABLA::Bytegen::BitwiseTypes::AND: expectedIns.bytes[0] = addOppCode(INS_AND, setup); break;
            case NABLA::Bytegen::BitwiseTypes::OR : expectedIns.bytes[0] = addOppCode(INS_OR , setup); break; 
            case NABLA::Bytegen::BitwiseTypes::XOR: expectedIns.bytes[0] = addOppCode(INS_XOR, setup); break;
            case NABLA::Bytegen::BitwiseTypes::NOT: expectedIns.bytes[0] = addOppCode(INS_NOT, setup); break;
            default:     FAIL("Something bad happened");  break; // Keep that compiler happy.
        }

        NABLA::Bytegen::Instruction ins = byteGen.createBitwiseInstruction(
            type,
            setup,
            destReg,
            arg1,
            arg2
        );

        // Build expected ins
        CHECK_EQUAL_TEXT(expectedIns.bytes[0], ins.bytes[0], "Opcode fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[1], ins.bytes[1], "Dest   fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[2], ins.bytes[2], "Byte 2 fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[3], ins.bytes[3], "Byte 3 fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[4], ins.bytes[4], "Byte 4 fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[5], ins.bytes[5], "Byte 5 fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[6], ins.bytes[6], "Byte 6 fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[7], ins.bytes[7], "Byte 7 fail");
    }
}