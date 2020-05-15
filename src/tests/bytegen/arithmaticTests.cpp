#include <iostream>
#include "bytegen.hpp"
#include "VSysInstructions.hpp"
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
                std::cerr << "Someone tried something silly with : " << reg  << ". IN THE ARITH TEST!" << std::endl;
                exit(EXIT_FAILURE); 
                break;
        }
    }
}

TEST_GROUP(ArithmaticTests)
{   
    NABLA::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ArithmaticTests, AllArithmatics)
{
    int setupMod = 0;

    for(int16_t j = 0; j < 1000; j++)
    {
        setupMod++;
        int16_t destReg = integerToRegister(getRandom16(0,15));

        int16_t arg1;
        int16_t arg2;

        NABLA::Bytegen::ArithmaticTypes type  =  static_cast<NABLA::Bytegen::ArithmaticTypes>(getRandom16(0,7));

        NABLA::Bytegen::ArithmaticSetup setup;

        NABLA::Bytegen::Instruction expectedIns;

        if(setupMod > 4 ) { setupMod = 1; }

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
            arg2 = getRandom16(0, 9);      // REG

            expectedIns.bytes[1] = integerToRegister(destReg);

            uint16_t num = static_cast<uint16_t>(arg1);
            expectedIns.bytes[2] = ( (num & 0xFF00) >> 8 ) ;
            expectedIns.bytes[3] = ( (num & 0x00FF) >> 0 ) ;
            expectedIns.bytes[4] = integerToRegister(arg2);
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;
        }

        if(setupMod == 3)
        {
            setup = NABLA::Bytegen::ArithmaticSetup::REG_NUM; 
            arg1 = getRandom16(0, 9);      // REG
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
            arg1 = getRandom16(0, 9);      // REG
            arg2 = getRandom16(0, 9);      // REG

            expectedIns.bytes[1] = integerToRegister(destReg);
            expectedIns.bytes[2] = integerToRegister(arg1);
            expectedIns.bytes[3] = integerToRegister(arg2);
            expectedIns.bytes[4] = 0xFF;
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;
        }


        // Set the expected op + ins
        switch(type)
        {
            case NABLA::Bytegen::ArithmaticTypes::ADD : expectedIns.bytes[0] = addOppCode(NABLA::VSYS::INS_ADD , setup); break;
            case NABLA::Bytegen::ArithmaticTypes::ADDD: expectedIns.bytes[0] = addOppCode(NABLA::VSYS::INS_ADDD, setup); break; 
            case NABLA::Bytegen::ArithmaticTypes::MUL : expectedIns.bytes[0] = addOppCode(NABLA::VSYS::INS_MUL , setup); break;
            case NABLA::Bytegen::ArithmaticTypes::MULD: expectedIns.bytes[0] = addOppCode(NABLA::VSYS::INS_MULD, setup); break; 
            case NABLA::Bytegen::ArithmaticTypes::DIV : expectedIns.bytes[0] = addOppCode(NABLA::VSYS::INS_DIV , setup); break;
            case NABLA::Bytegen::ArithmaticTypes::DIVD: expectedIns.bytes[0] = addOppCode(NABLA::VSYS::INS_DIVD, setup); break; 
            case NABLA::Bytegen::ArithmaticTypes::SUB : expectedIns.bytes[0] = addOppCode(NABLA::VSYS::INS_SUB , setup); break;
            case NABLA::Bytegen::ArithmaticTypes::SUBD: expectedIns.bytes[0] = addOppCode(NABLA::VSYS::INS_SUBD, setup); break; 
            default:     FAIL("Something bad happened");  break; // Keep that compiler happy.
        }

        NABLA::Bytegen::Instruction ins = byteGen.createArithmaticInstruction(
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