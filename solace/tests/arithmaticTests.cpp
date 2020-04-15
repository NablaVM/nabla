#include <iostream>
#include "bytegen.hpp"
#include "InsManifest.hpp"
#include <random>
#include "CppUTest/TestHarness.h"

namespace
{
    uint8_t addOppCode(uint8_t ins, SOLACE::Bytegen::ArithmaticSetup setup)
    {
        if(setup == SOLACE::Bytegen::ArithmaticSetup::REG_REG){ return (ins | 0x00); }
        if(setup == SOLACE::Bytegen::ArithmaticSetup::REG_NUM){ return (ins | 0x01); }
        if(setup == SOLACE::Bytegen::ArithmaticSetup::NUM_REG){ return (ins | 0x02); }
        if(setup == SOLACE::Bytegen::ArithmaticSetup::NUM_NUM){ return (ins | 0x03); }
        return ins;
    }

    uint16_t getRandom16(uint16_t low, uint16_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        return dis(gen);
    }
}

TEST_GROUP(ArithmaticTests)
{   
    SOLACE::Bytegen byteGen;
    
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(ArithmaticTests, AllArithmatics)
{
    int setupMod = 0;
    int16_t destReg = 0;

    for(int16_t j = 0; j < 1000; j++)
    {
        setupMod++;
        destReg++;

        int16_t arg1;
        int16_t arg2;

        SOLACE::Bytegen::ArithmaticTypes type  =  static_cast<SOLACE::Bytegen::ArithmaticTypes>(getRandom16(0,7));

        SOLACE::Bytegen::ArithmaticSetup setup;

        SOLACE::Bytegen::Instruction expectedIns;

        if(setupMod > 4 ) { setupMod = 0; }

        if(setupMod == 1)
        { 
            setup = SOLACE::Bytegen::ArithmaticSetup::NUM_NUM; 
            arg1 = getRandom16(0, 60000);   // NUM
            arg2 = getRandom16(0, 60000);   // NUM

            expectedIns.bytes[1] = destReg;
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
            setup = SOLACE::Bytegen::ArithmaticSetup::NUM_REG; 
            arg1 = getRandom16(0, 60000);   // NUM
            arg2 = getRandom16(0, 15);      // REG

            expectedIns.bytes[1] = destReg;

            uint16_t num = static_cast<uint16_t>(arg1);
            expectedIns.bytes[2] = ( (num & 0xFF00) >> 8 ) ;
            expectedIns.bytes[3] = ( (num & 0x00FF) >> 0 ) ;
            expectedIns.bytes[4] = arg2;
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;
        }

        if(setupMod == 3)
        {
            setup = SOLACE::Bytegen::ArithmaticSetup::REG_NUM; 
            arg1 = getRandom16(0, 15);      // REG
            arg2 = getRandom16(0, 60000);   // NUM

            expectedIns.bytes[1] = destReg;
            expectedIns.bytes[2] = arg1;
            uint16_t num = static_cast<uint16_t>(arg2);
            expectedIns.bytes[3] = ( (num & 0xFF00) >> 8 ) ;
            expectedIns.bytes[4] = ( (num & 0x00FF) >> 0 ) ;
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;
        }

        if(setupMod == 4)
        {
            setup = SOLACE::Bytegen::ArithmaticSetup::REG_REG; 
            arg1 = getRandom16(0, 15);      // REG
            arg2 = getRandom16(0, 15);      // REG

            expectedIns.bytes[1] = destReg;
            
            expectedIns.bytes[2] = arg1;
            expectedIns.bytes[3] = arg2;
            expectedIns.bytes[4] = 0xFF;
            expectedIns.bytes[5] = 0xFF;
            expectedIns.bytes[6] = 0xFF;
            expectedIns.bytes[7] = 0xFF;
        }


        // Set the expected op + ins
        switch(type)
        {
            case SOLACE::Bytegen::ArithmaticTypes::ADD : expectedIns.bytes[0] = addOppCode(SOLACE::MANIFEST::INS_ADD , setup); break;
            case SOLACE::Bytegen::ArithmaticTypes::ADDD: expectedIns.bytes[0] = addOppCode(SOLACE::MANIFEST::INS_ADDD, setup); break; 
            case SOLACE::Bytegen::ArithmaticTypes::MUL : expectedIns.bytes[0] = addOppCode(SOLACE::MANIFEST::INS_MUL , setup); break;
            case SOLACE::Bytegen::ArithmaticTypes::MULD: expectedIns.bytes[0] = addOppCode(SOLACE::MANIFEST::INS_MULD, setup); break; 
            case SOLACE::Bytegen::ArithmaticTypes::DIV : expectedIns.bytes[0] = addOppCode(SOLACE::MANIFEST::INS_DIV , setup); break;
            case SOLACE::Bytegen::ArithmaticTypes::DIVD: expectedIns.bytes[0] = addOppCode(SOLACE::MANIFEST::INS_DIVD, setup); break; 
            case SOLACE::Bytegen::ArithmaticTypes::SUB : expectedIns.bytes[0] = addOppCode(SOLACE::MANIFEST::INS_SUB , setup); break;
            case SOLACE::Bytegen::ArithmaticTypes::SUBD: expectedIns.bytes[0] = addOppCode(SOLACE::MANIFEST::INS_SUBD, setup); break; 
            default:     FAIL("Something bad happened");  break; // Keep that compiler happy.
        }

        if(destReg == 16) { destReg = 0; }

        SOLACE::Bytegen::Instruction ins = byteGen.createArithmaticInstruction(
            type,
            setup,
            destReg,
            arg1,
            arg2
        );

        // Build expected ins
        CHECK_EQUAL_TEXT(expectedIns.bytes[0], expectedIns.bytes[0], "Opcode fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[1], expectedIns.bytes[1], "Dest   fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[2], expectedIns.bytes[2], "Byte 2 fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[3], expectedIns.bytes[3], "Byte 3 fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[4], expectedIns.bytes[4], "Byte 4 fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[5], expectedIns.bytes[5], "Byte 5 fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[6], expectedIns.bytes[6], "Byte 6 fail");
        CHECK_EQUAL_TEXT(expectedIns.bytes[7], expectedIns.bytes[7], "Byte 7 fail");
    }
}