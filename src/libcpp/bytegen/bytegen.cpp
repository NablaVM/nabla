#include "bytegen.hpp"

#include "VSysInstructions.hpp"
#include <iostream>
#include <bitset>
#include <ieee754.h>

namespace NABLA
{
    namespace
    {
        // This might seem silly, but it allows us to change register addresses without picking out 
        // ALL the aread that they are being set
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
                    std::cerr << "Someone tried something silly with : " << reg  << std::endl;
                    exit(EXIT_FAILURE); 
                    break;
            }
        }

        // Get a stack address given the stack type
        uint8_t getStackAddress(Bytegen::Stacks stack)
        {
            switch(stack)
            {
                case Bytegen::Stacks::GLOBAL:
                    return NABLA::VSYS::GLOBAL_STACK;
                case Bytegen::Stacks::LOCAL:
                    return NABLA::VSYS::LOCAL_STACK;
                default:
                    std::cerr << "Someone tried something silly awful with getStackAddress" << std::endl;
                    exit(EXIT_FAILURE); 
                    return 0;
            }
            return 0; // Keep the compiler happy
        }

        // Helper for debugging
        void dumpInstruction(Bytegen::Instruction ins)
        {
            std::cout << std::endl << "\t"
            << std::bitset<8>(ins.bytes[0]) << " | " 
            << std::bitset<8>(ins.bytes[1]) << " | " 
            << std::bitset<8>(ins.bytes[2]) << " | " 
            << std::bitset<8>(ins.bytes[3]) << " | " 
            << std::bitset<8>(ins.bytes[4]) << " | " 
            << std::bitset<8>(ins.bytes[5]) << " | " 
            << std::bitset<8>(ins.bytes[6]) << " | " 
            << std::bitset<8>(ins.bytes[7]) << "   " 
            << std::endl << std::endl;
        }
    }

    // ------------------------------------------------------------------------
    // Bytegen
    // ------------------------------------------------------------------------
    
    Bytegen::Bytegen() : functionCounter(0)
    {
        //displayManifest();

    }

    // ------------------------------------------------------------------------
    // getCurrentFunctionCouner
    // ------------------------------------------------------------------------
    
    uint32_t Bytegen::getCurrentFunctionCouner() const
    {
        return functionCounter;
    }
    
    // ------------------------------------------------------------------------
    // createFunctionStart
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createFunctionStart(std::string name, uint64_t numInstructions, uint32_t &address)
    {
        Instruction ins;

        address = functionCounter;

        ins.bytes[0] = NABLA::VSYS::INS_FUNCTION_CREATE;
        ins.bytes[1] = (numInstructions & 0x00FF000000000000) >> 48 ;
        ins.bytes[2] = (numInstructions & 0x0000FF0000000000) >> 40 ;
        ins.bytes[3] = (numInstructions & 0x000000FF00000000) >> 32 ;
        ins.bytes[4] = (numInstructions & 0x00000000FF000000) >> 24 ;
        ins.bytes[5] = (numInstructions & 0x0000000000FF0000) >> 16 ;
        ins.bytes[6] = (numInstructions & 0x000000000000FF00) >> 8  ;
        ins.bytes[7] = (numInstructions & 0x00000000000000FF) >> 0  ;

        functionCounter++;

        return ins;
    }

    // ------------------------------------------------------------------------
    // createFunctionEnd
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createFunctionEnd()
    {
        //std::cout << "Bytegen::createFunctionEnd()" << std::endl;

        return Instruction{ NABLA::VSYS::INS_FUNCTION_END, 
                            0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF 
        };
    }

    // ------------------------------------------------------------------------
    // createConstantString
    // ------------------------------------------------------------------------
    
    std::vector<uint8_t> Bytegen::createConstantString(std::string val)
    {
        std::vector<uint8_t> result;

//        std::cout << "Bytegen::createConstantString(" << val << ")" << std::endl;
        
        result.push_back( NABLA::VSYS::CONST_STR );

        uint8_t strSize = val.size();

        // Add the size
        result.push_back(strSize);

        // Ensure we only place the values we accounted for
        for(uint8_t i = 0; i < strSize; i++)
        {
            result.push_back(static_cast<uint8_t>(val[i]));
        }

        return result;
    }

    // ------------------------------------------------------------------------
    // createConstantInt
    // ------------------------------------------------------------------------
    
    std::vector<uint8_t> Bytegen::createConstantInt   (uint64_t val, Integers integerType)
    {
        std::vector<uint8_t> result;

        /*
            I'm sure there is a more clever way to do this, but right now I'm aiming for correct over clever.
            This can be revisited in the future.
        */
        switch(integerType)
        {
            case Integers::EIGHT:
            {
                result.push_back( NABLA::VSYS::CONST_INT  |  0x0 );
                result.push_back( (val & 0x000000FF) >> 0  );
                break;
            }
            case Integers::SIXTEEN:
            {
                result.push_back( NABLA::VSYS::CONST_INT  |  0x1 );
                result.push_back( (val & 0x0000FF00) >> 8  );
                result.push_back( (val & 0x000000FF) >> 0  );
                break;
            }
            case Integers::THIRTY_TWO:
            {
                result.push_back( NABLA::VSYS::CONST_INT  |  0x2 );
                result.push_back( (val & 0xFF000000) >> 24 );
                result.push_back( (val & 0x00FF0000) >> 16 );
                result.push_back( (val & 0x0000FF00) >> 8  );
                result.push_back( (val & 0x000000FF) >> 0  );
                break;
            }
            case Integers::SIXTY_FOUR:
            {
                result.push_back( NABLA::VSYS::CONST_INT  |  0x3 );
                result.push_back( (val & 0xFF00000000000000) >> 56 );
                result.push_back( (val & 0x00FF000000000000) >> 48 );
                result.push_back( (val & 0x0000FF0000000000) >> 40 );
                result.push_back( (val & 0x000000FF00000000) >> 32 );
                result.push_back( (val & 0x00000000FF000000) >> 24 );
                result.push_back( (val & 0x0000000000FF0000) >> 16 );
                result.push_back( (val & 0x000000000000FF00) >> 8  );
                result.push_back( (val & 0x00000000000000FF) >> 0  );
                break;
            }
        }

        return result;
    }

    // ------------------------------------------------------------------------
    // createConstantDouble
    // ------------------------------------------------------------------------
    
    std::vector<uint8_t> Bytegen::createConstantDouble(double dval)
    {
        std::vector<uint8_t> result;

        ieee754_double ied;

        ied.d = dval;

        uint64_t packed = (uint64_t)ied.ieee.negative  << 63| 
                          (uint64_t)ied.ieee.exponent  << 52|
                          (uint64_t)ied.ieee.mantissa0 << 32|
                          (uint64_t)ied.ieee.mantissa1 << 0;

        result.push_back( NABLA::VSYS::CONST_DBL      );
        result.push_back( (packed & 0xFF00000000000000) >> 56 );
        result.push_back( (packed & 0x00FF000000000000) >> 48 );
        result.push_back( (packed & 0x0000FF0000000000) >> 40 );
        result.push_back( (packed & 0x000000FF00000000) >> 32 );
        result.push_back( (packed & 0x00000000FF000000) >> 24 );
        result.push_back( (packed & 0x0000000000FF0000) >> 16 );
        result.push_back( (packed & 0x000000000000FF00) >> 8  );
        result.push_back( (packed & 0x00000000000000FF) >> 0  );

        return result;
    }

    // ------------------------------------------------------------------------
    // createConstantDouble
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createArithmaticInstruction(ArithmaticTypes type, ArithmaticSetup setup, int16_t arg1, int16_t arg2, int16_t arg3)
    {        
        Instruction ins;

        uint8_t op = 0;

        switch(type)
        {
            case Bytegen::ArithmaticTypes::ADD : op = NABLA::VSYS::INS_ADD ; break;
            case Bytegen::ArithmaticTypes::ADDD: op = NABLA::VSYS::INS_ADDD; break; 
            case Bytegen::ArithmaticTypes::MUL : op = NABLA::VSYS::INS_MUL ; break;
            case Bytegen::ArithmaticTypes::MULD: op = NABLA::VSYS::INS_MULD; break; 
            case Bytegen::ArithmaticTypes::DIV : op = NABLA::VSYS::INS_DIV ; break;
            case Bytegen::ArithmaticTypes::DIVD: op = NABLA::VSYS::INS_DIVD; break; 
            case Bytegen::ArithmaticTypes::SUB : op = NABLA::VSYS::INS_SUB ; break;
            case Bytegen::ArithmaticTypes::SUBD: op = NABLA::VSYS::INS_SUBD; break; 
            default:      exit(EXIT_FAILURE);  return ins; // Keep that compiler happy.
        }

        // Set id
        switch(setup)
        {
            case Bytegen::ArithmaticSetup::REG_REG: 
            {
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));
                ins.bytes[2] = (integerToRegister(arg2));
                ins.bytes[3] = (integerToRegister(arg3));
                ins.bytes[4] = 0xFF;
                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break; 
            }

            case Bytegen::ArithmaticSetup::NUM_REG: 
            {
                op = op | 0x02; 
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));

                uint16_t num = static_cast<uint16_t>(arg2);

                ins.bytes[2] = ( (num & 0xFF00) >> 8 ) ;

                ins.bytes[3] = ( (num & 0x00FF) >> 0 ) ;

                ins.bytes[4] = (integerToRegister(arg3));

                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break; 
            }

            case Bytegen::ArithmaticSetup::REG_NUM: 
            {
                op = op | 0x01; 
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));
                ins.bytes[2] = (integerToRegister(arg2));

                uint16_t num = static_cast<uint16_t>(arg3);

                ins.bytes[3] = ( (num & 0xFF00) >> 8 ) ;

                ins.bytes[4] = ( (num & 0x00FF) >> 0 ) ;

                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break; 
            }

            case Bytegen::ArithmaticSetup::NUM_NUM: 
            {
                op = op | 0x03; 
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));

                uint16_t num = static_cast<uint16_t>(arg2);

                ins.bytes[2] = ( (num & 0xFF00) >> 8 ) ;
                ins.bytes[3] = ( (num & 0x00FF) >> 0 ) ;

                uint16_t num1 = static_cast<uint16_t>(arg3);

                ins.bytes[4] = ( (num1 & 0xFF00) >> 8 ) ;
                ins.bytes[5] = ( (num1 & 0x00FF) >> 0 ) ;

                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break; 
            }

            default:      
                std::cerr << "SOMETHING AWFUL HAPPENED DON'T TRUST ANYTHING" << std::endl;
                return ins; // Keep that compiler happy.
        }

     //   //std::cout << "Arg1: " << arg1 << " Arg2: " << arg2 << " Arg3: " << arg3 << std::endl;
     //   dumpInstruction(ins);

        return ins;
    }


    // ------------------------------------------------------------------------
    // createBranchInstruction
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createBranchInstruction(BranchTypes type, uint8_t reg1, uint8_t reg2, uint32_t location)
    {
        Instruction ins;

        // Set the instruction op
        switch(type)
        {
            case Bytegen::BranchTypes::BGT  : ins.bytes[0] = NABLA::VSYS::INS_BGT  ; break; 
            case Bytegen::BranchTypes::BGTE : ins.bytes[0] = NABLA::VSYS::INS_BGTE ; break; 
            case Bytegen::BranchTypes::BLT  : ins.bytes[0] = NABLA::VSYS::INS_BLT  ; break; 
            case Bytegen::BranchTypes::BLTE : ins.bytes[0] = NABLA::VSYS::INS_BLTE ; break; 
            case Bytegen::BranchTypes::BEQ  : ins.bytes[0] = NABLA::VSYS::INS_BEQ  ; break; 
            case Bytegen::BranchTypes::BNE  : ins.bytes[0] = NABLA::VSYS::INS_BNE  ; break; 
            case Bytegen::BranchTypes::BGTD : ins.bytes[0] = NABLA::VSYS::INS_BGTD ; break;
            case Bytegen::BranchTypes::BGTED: ins.bytes[0] = NABLA::VSYS::INS_BGTED; break;
            case Bytegen::BranchTypes::BLTD : ins.bytes[0] = NABLA::VSYS::INS_BLTD ; break;
            case Bytegen::BranchTypes::BLTED: ins.bytes[0] = NABLA::VSYS::INS_BLTED; break;
            case Bytegen::BranchTypes::BEQD : ins.bytes[0] = NABLA::VSYS::INS_BEQD ; break;
            case Bytegen::BranchTypes::BNED : ins.bytes[0] = NABLA::VSYS::INS_BNED ; break;
            default:                          return ins; // Keep that compiler happy.
        }

        ins.bytes[1] = integerToRegister(reg1);
        ins.bytes[2] = integerToRegister(reg2);
        ins.bytes[3] = (location & 0xFF000000) >> 24 ;
        ins.bytes[4] = (location & 0x00FF0000) >> 16 ;
        ins.bytes[5] = (location & 0x0000FF00) >> 8  ;
        ins.bytes[6] = (location & 0x000000FF) >> 0  ;
        ins.bytes[7] = 0xFF;

        //dumpInstruction(ins);

        return ins;
    }

    // ------------------------------------------------------------------------
    // createJumpInstruction
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createJumpInstruction(uint32_t location)
    {
        Instruction ins;

        ins.bytes[0] = NABLA::VSYS::INS_JUMP;
        ins.bytes[1] = (location & 0xFF000000) >> 24 ;
        ins.bytes[2] = (location & 0x00FF0000) >> 16 ;
        ins.bytes[3] = (location & 0x0000FF00) >> 8  ;
        ins.bytes[4] = (location & 0x000000FF) >> 0  ;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;
        
        //dumpInstruction(ins);

        return ins;
    }

    // ------------------------------------------------------------------------
    // createMovInstruction
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createMovInstruction(Bytegen::MovSetup setup, uint8_t reg1, uint32_t arg2)
    {
        
        Instruction ins;

        if(setup == Bytegen::MovSetup::REG_REG)
        {
            ins.bytes[0] = NABLA::VSYS::INS_MOV;
            ins.bytes[1] = integerToRegister(reg1) ;
            ins.bytes[2] = integerToRegister(arg2) ;
            ins.bytes[3] = 0xFF;
            ins.bytes[4] = 0xFF;
            ins.bytes[5] = 0xFF;
            ins.bytes[6] = 0xFF;
            ins.bytes[7] = 0xFF;
        }
        else
        {
            ins.bytes[0] = (NABLA::VSYS::INS_MOV | 0x01);
            ins.bytes[1] = integerToRegister(reg1) ;
            ins.bytes[2] = (arg2 & 0xFF000000) >> 24 ;
            ins.bytes[3] = (arg2 & 0x00FF0000) >> 16 ;
            ins.bytes[4] = (arg2 & 0x0000FF00) >> 8  ;
            ins.bytes[5] = (arg2 & 0x000000FF) >> 0  ;
            ins.bytes[6] = 0xFF;
            ins.bytes[7] = 0xFF;
        }
        
       // dumpInstruction(ins);

        return ins;
    }

    // ------------------------------------------------------------------------
    // createPushInstruction
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createPushInstruction(Stacks stack, uint8_t reg)
    {
        Instruction ins;
        ins.bytes[0] = NABLA::VSYS::INS_PUSH;
        ins.bytes[1] = getStackAddress(stack);
        ins.bytes[2] = integerToRegister(reg);
        ins.bytes[3] = 0xFF;
        ins.bytes[4] = 0xFF;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;

        //dumpInstruction(ins);

        return ins;
    }

    // ------------------------------------------------------------------------
    // createPopInstruction
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createPopInstruction(Stacks stack, uint8_t reg)
    {
        Instruction ins;
        ins.bytes[0] = NABLA::VSYS::INS_POP;
        ins.bytes[1] = integerToRegister(reg);
        ins.bytes[2] = getStackAddress(stack);
        ins.bytes[3] = 0xFF;
        ins.bytes[4] = 0xFF;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;

        //dumpInstruction(ins);
        
        return ins;
    }


    // ------------------------------------------------------------------------
    // createPushwInstruction
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createPushwInstruction(Stacks stack, uint8_t reg)
    {
        Instruction ins;
        ins.bytes[0] = NABLA::VSYS::INS_PUSHW;
        ins.bytes[1] = getStackAddress(stack);
        ins.bytes[2] = integerToRegister(reg);
        ins.bytes[3] = 0xFF;
        ins.bytes[4] = 0xFF;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;

        //dumpInstruction(ins);

        return ins;
    }

    // ------------------------------------------------------------------------
    // createPopwInstruction
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createPopwInstruction(Stacks stack, uint8_t reg)
    {
        Instruction ins;
        ins.bytes[0] = NABLA::VSYS::INS_POPW;
        ins.bytes[1] = integerToRegister(reg);
        ins.bytes[2] = getStackAddress(stack);
        ins.bytes[3] = 0xFF;
        ins.bytes[4] = 0xFF;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;

        //dumpInstruction(ins);
        
        return ins;
    }

    // ------------------------------------------------------------------------
    // createStbInstruction
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createStbInstruction(Stacks stack, LoadStoreSetup setup, uint32_t location, uint8_t reg)
    {
        Instruction ins;

        switch(setup)
        {
            case Bytegen::LoadStoreSetup::NUMBER_BASED:
            {
                ins.bytes[0] = NABLA::VSYS::INS_STB;
                ins.bytes[1] = getStackAddress(stack);
                ins.bytes[2] = (location & 0xFF000000) >> 24 ;
                ins.bytes[3] = (location & 0x00FF0000) >> 16 ;
                ins.bytes[4] = (location & 0x0000FF00) >> 8  ;
                ins.bytes[5] = (location & 0x000000FF) >> 0  ;
                ins.bytes[6] = integerToRegister(reg);
                ins.bytes[7] = 0xFF;
                break;
            }

            case Bytegen::LoadStoreSetup::REGISTER_BASED:
            {
                ins.bytes[0] = (NABLA::VSYS::INS_STB | 0x01);
                ins.bytes[1] = getStackAddress(stack);
                ins.bytes[2] = integerToRegister(location);
                ins.bytes[3] = integerToRegister(reg);
                ins.bytes[4] = 0xFF;
                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break;
            }
            default:
                std::cerr << "Someone is doing something wrong with bytegen createStbInstruction!" << std::endl;
                exit(EXIT_FAILURE); 
                break;
        }


        //dumpInstruction(ins);

        return ins;
    }

    // ------------------------------------------------------------------------
    // createStwInstruction
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createStwInstruction(Stacks stack, LoadStoreSetup setup, uint32_t location, uint8_t reg)
    {
        Instruction ins;

        switch(setup)
        {
            case Bytegen::LoadStoreSetup::NUMBER_BASED:
            {
                ins.bytes[0] = NABLA::VSYS::INS_STW;
                ins.bytes[1] = getStackAddress(stack);
                ins.bytes[2] = (location & 0xFF000000) >> 24 ;
                ins.bytes[3] = (location & 0x00FF0000) >> 16 ;
                ins.bytes[4] = (location & 0x0000FF00) >> 8  ;
                ins.bytes[5] = (location & 0x000000FF) >> 0  ;
                ins.bytes[6] = integerToRegister(reg);
                ins.bytes[7] = 0xFF;
                break;
            }

            case Bytegen::LoadStoreSetup::REGISTER_BASED:
            {
                ins.bytes[0] = (NABLA::VSYS::INS_STW | 0x01);
                ins.bytes[1] = getStackAddress(stack);
                ins.bytes[2] = integerToRegister(location);
                ins.bytes[3] = integerToRegister(reg);
                ins.bytes[4] = 0xFF;
                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break;
            }
            default:
                std::cerr << "Someone is doing something wrong with bytegen createStwInstruction!" << std::endl;
                exit(EXIT_FAILURE); 
                break;
        }


        //dumpInstruction(ins);

        return ins;
    }

    // ------------------------------------------------------------------------
    // createLdbInstruction
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createLdbInstruction(Stacks stack, LoadStoreSetup setup, uint32_t location, uint8_t reg)
    {
        Instruction ins;

        switch(setup)
        {
            case Bytegen::LoadStoreSetup::NUMBER_BASED:
            {
                ins.bytes[0] = NABLA::VSYS::INS_LDB;
                ins.bytes[1] = integerToRegister(reg);
                ins.bytes[2] = getStackAddress(stack);
                ins.bytes[3] = (location & 0xFF000000) >> 24 ;
                ins.bytes[4] = (location & 0x00FF0000) >> 16 ;
                ins.bytes[5] = (location & 0x0000FF00) >> 8  ;
                ins.bytes[6] = (location & 0x000000FF) >> 0  ;
                ins.bytes[7] = 0xFF;
                break;
            }

            case Bytegen::LoadStoreSetup::REGISTER_BASED:
            {
                ins.bytes[0] = (NABLA::VSYS::INS_LDB | 0x01);
                ins.bytes[1] = integerToRegister(reg);
                ins.bytes[2] = getStackAddress(stack);
                ins.bytes[3] = integerToRegister(location);
                ins.bytes[4] = 0xFF;
                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break;
            }

            default:
                std::cerr << "Someone is doing something wrong with bytegen createLdbInstruction!" << std::endl;
                exit(EXIT_FAILURE); 
                break;
        }

        //dumpInstruction(ins);

        return ins;
    }

    // ------------------------------------------------------------------------
    // createLdwInstruction
    // ------------------------------------------------------------------------
    
    Bytegen::Instruction Bytegen::createLdwInstruction(Stacks stack, LoadStoreSetup setup, uint32_t location, uint8_t reg)
    {
        Instruction ins;

        switch(setup)
        {
            case Bytegen::LoadStoreSetup::NUMBER_BASED:
            {
                ins.bytes[0] = NABLA::VSYS::INS_LDW;
                ins.bytes[1] = integerToRegister(reg);
                ins.bytes[2] = getStackAddress(stack);
                ins.bytes[3] = (location & 0xFF000000) >> 24 ;
                ins.bytes[4] = (location & 0x00FF0000) >> 16 ;
                ins.bytes[5] = (location & 0x0000FF00) >> 8  ;
                ins.bytes[6] = (location & 0x000000FF) >> 0  ;
                ins.bytes[7] = 0xFF;
                break;
            }

            case Bytegen::LoadStoreSetup::REGISTER_BASED:
            {
                ins.bytes[0] = (NABLA::VSYS::INS_LDW | 0x01);
                ins.bytes[1] = integerToRegister(reg);
                ins.bytes[2] = getStackAddress(stack);
                ins.bytes[3] = integerToRegister(location);
                ins.bytes[4] = 0xFF;
                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break;
            }

            default:
                std::cerr << "Someone is doing something wrong with bytegen createLdwInstruction!" << std::endl;
                exit(EXIT_FAILURE); 
                break;
        }

        //dumpInstruction(ins);

        return ins;
    }

    // ------------------------------------------------------------------------
    // createReturnInstruction
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createReturnInstruction()
    {
        Instruction ins;
        ins.bytes[0] = NABLA::VSYS::INS_RET;
        ins.bytes[1] = 0xFF;
        ins.bytes[2] = 0xFF;
        ins.bytes[3] = 0xFF;
        ins.bytes[4] = 0xFF;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;

        //dumpInstruction(ins);
        
        return ins;
    }

    // ------------------------------------------------------------------------
    // createExitInstruction
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createExitInstruction()
    {
        Instruction ins;
        ins.bytes[0] = NABLA::VSYS::INS_EXIT;
        ins.bytes[1] = 0xFF;
        ins.bytes[2] = 0xFF;
        ins.bytes[3] = 0xFF;
        ins.bytes[4] = 0xFF;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;

        //dumpInstruction(ins);
        
        return ins;
    }

    // ------------------------------------------------------------------------
    // createYieldInstruction
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createYieldInstruction()
    {
        Instruction ins;
        ins.bytes[0] = NABLA::VSYS::INS_YIELD;
        ins.bytes[1] = 0xFF;
        ins.bytes[2] = 0xFF;
        ins.bytes[3] = 0xFF;
        ins.bytes[4] = 0xFF;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;

        //dumpInstruction(ins);
        
        return ins;
    }

    // ------------------------------------------------------------------------
    // createCallInstruction
    // ------------------------------------------------------------------------

    std::vector<Bytegen::Instruction> Bytegen::createCallInstruction(uint32_t funcFrom, uint32_t ret, uint32_t address)
    {
        std::vector<Instruction> ins;

        Instruction cssf;
        cssf.bytes[0] = NABLA::VSYS::INS_CS_SF;
        cssf.bytes[1] = (funcFrom & 0xFF000000) >> 24;
        cssf.bytes[2] = (funcFrom & 0x00FF0000) >> 16;
        cssf.bytes[3] = (funcFrom & 0x0000FF00) >> 8 ;
        cssf.bytes[4] = (funcFrom & 0x000000FF) >> 0 ;
        cssf.bytes[5] = 0xFF;
        cssf.bytes[6] = 0xFF;
        cssf.bytes[7] = 0xFF;

        ins.push_back(cssf);

        Instruction cssr;
        cssr.bytes[0] = NABLA::VSYS::INS_CS_SR;
        cssr.bytes[1] = (ret & 0xFF000000) >> 24;
        cssr.bytes[2] = (ret & 0x00FF0000) >> 16;
        cssr.bytes[3] = (ret & 0x0000FF00) >> 8 ;
        cssr.bytes[4] = (ret & 0x000000FF) >> 0 ;
        cssr.bytes[5] = 0xFF;
        cssr.bytes[6] = 0xFF;
        cssr.bytes[7] = 0xFF;

        ins.push_back(cssr);

        Instruction call;

        call.bytes[0] = NABLA::VSYS::INS_CALL;
        call.bytes[1] = (address & 0xFF000000) >> 24;
        call.bytes[2] = (address & 0x00FF0000) >> 16;
        call.bytes[3] = (address & 0x0000FF00) >> 8 ;
        call.bytes[4] = (address & 0x000000FF) >> 0 ;
        call.bytes[5] = 0xFF;
        call.bytes[6] = 0xFF;
        call.bytes[7] = 0xFF;

        ins.push_back(call);

        return ins;
    }

    // ------------------------------------------------------------------------
    // createPcallInstruction
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createPcallInstruction(uint32_t address)
    {
        Instruction ins;
        ins.bytes[0] = NABLA::VSYS::INS_PCALL;
        ins.bytes[1] = (address & 0xFF000000) >> 24;
        ins.bytes[2] = (address & 0x00FF0000) >> 16;
        ins.bytes[3] = (address & 0x0000FF00) >> 8 ;
        ins.bytes[4] = (address & 0x000000FF) >> 0 ;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;
        return ins;
    }

    // ------------------------------------------------------------------------
    // createSegConstInstruction
    // ------------------------------------------------------------------------

    std::vector<uint8_t> Bytegen::createSegConstInstruction(uint64_t count)
    {
        std::vector<uint8_t> result;

        result.push_back( NABLA::VSYS::INS_SEG_CONST );
        result.push_back( (count & 0xFF00000000000000) >> 56 );
        result.push_back( (count & 0x00FF000000000000) >> 48 );
        result.push_back( (count & 0x0000FF0000000000) >> 40 );
        result.push_back( (count & 0x000000FF00000000) >> 32 );
        result.push_back( (count & 0x00000000FF000000) >> 24 );
        result.push_back( (count & 0x0000000000FF0000) >> 16 );
        result.push_back( (count & 0x000000000000FF00) >> 8  );
        result.push_back( (count & 0x00000000000000FF) >> 0  );

        return result;
    }

    // ------------------------------------------------------------------------
    // createSegFuncInstruction
    // ------------------------------------------------------------------------

    std::vector<uint8_t> Bytegen::createSegFuncInstruction(uint64_t entryAddress)
    {
        std::vector<uint8_t> result;

        result.push_back( NABLA::VSYS::INS_SEG_FUNC );
        result.push_back( (entryAddress & 0xFF00000000000000) >> 56 );
        result.push_back( (entryAddress & 0x00FF000000000000) >> 48 );
        result.push_back( (entryAddress & 0x0000FF0000000000) >> 40 );
        result.push_back( (entryAddress & 0x000000FF00000000) >> 32 );
        result.push_back( (entryAddress & 0x00000000FF000000) >> 24 );
        result.push_back( (entryAddress & 0x0000000000FF0000) >> 16 );
        result.push_back( (entryAddress & 0x000000000000FF00) >> 8  );
        result.push_back( (entryAddress & 0x00000000000000FF) >> 0  );

        return result;
    }
    
    // ------------------------------------------------------------------------
    // createSegBinEOF
    // ------------------------------------------------------------------------

    std::vector<uint8_t> Bytegen::createSegBinEOF()
    {
        std::vector<uint8_t> result;

        // In the future I'd like to dump a checksum here so we are using
        // a vector. For now, we just load 0xFF
        result.push_back( NABLA::VSYS::INS_SEG_BEOF );
        result.push_back( 0xFF );
        result.push_back( 0xFF );
        result.push_back( 0xFF );
        result.push_back( 0xFF );
        result.push_back( 0xFF );
        result.push_back( 0xFF );
        result.push_back( 0xFF );

        return result;
    }

    // ------------------------------------------------------------------------
    // 
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createBitwiseInstruction(Bytegen::BitwiseTypes type, Bytegen::ArithmaticSetup setup, int16_t arg1, int16_t arg2, int16_t arg3)
    {
        uint8_t op;
        Instruction ins;

        switch(type)
        {
            case Bytegen::BitwiseTypes::LSH : op = NABLA::VSYS::INS_LSH; break; 
            case Bytegen::BitwiseTypes::RSH : op = NABLA::VSYS::INS_RSH; break; 
            case Bytegen::BitwiseTypes::AND : op = NABLA::VSYS::INS_AND; break; 
            case Bytegen::BitwiseTypes::OR  : op = NABLA::VSYS::INS_OR ; break; 
            case Bytegen::BitwiseTypes::XOR : op = NABLA::VSYS::INS_XOR; break; 
            case Bytegen::BitwiseTypes::NOT : op = NABLA::VSYS::INS_NOT; break;
            default:  std::cerr << "Type error - Bytegen bitwiseInstruction" << std::endl; 
                      exit(EXIT_FAILURE); // Keep that compiler happy.
        }

  // Set id
        switch(setup)
        {
            case Bytegen::ArithmaticSetup::REG_REG: 
            {
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));
                ins.bytes[2] = (integerToRegister(arg2));
                ins.bytes[3] = (integerToRegister(arg3));
                ins.bytes[4] = 0xFF;
                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                
                if(type == BitwiseTypes::NOT)
                {
                    ins.bytes[3] = 0xFF;
                }
                break; 
            }

            case Bytegen::ArithmaticSetup::NUM_REG: 
            {
                op = op | 0x02; 
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));

                uint16_t num = static_cast<uint16_t>(arg2);

                ins.bytes[2] = ( (num & 0xFF00) >> 8 ) ;

                ins.bytes[3] = ( (num & 0x00FF) >> 0 ) ;

                ins.bytes[4] = (integerToRegister(arg3));

                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;


                if(type == BitwiseTypes::NOT)
                {
                    ins.bytes[4] = 0xFF;
                }
                break; 
            }

            case Bytegen::ArithmaticSetup::REG_NUM: 
            {
                if(type == BitwiseTypes::NOT)
                {
                    std::cerr << "Someone is trying to use NOT incorrectly in the ByteGen!" << std::endl;
                    exit(EXIT_FAILURE);
                }

                op = op | 0x01; 
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));
                ins.bytes[2] = (integerToRegister(arg2));

                uint16_t num = static_cast<uint16_t>(arg3);

                ins.bytes[3] = ( (num & 0xFF00) >> 8 ) ;

                ins.bytes[4] = ( (num & 0x00FF) >> 0 ) ;

                ins.bytes[5] = 0xFF;
                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break; 
            }

            case Bytegen::ArithmaticSetup::NUM_NUM: 
            {
                if(type == BitwiseTypes::NOT)
                {
                    std::cerr << "Someone is trying to use NOT incorrectly in the ByteGen!" << std::endl;
                    exit(EXIT_FAILURE);
                }

                op = op | 0x03; 
                ins.bytes[0] = (op);
                ins.bytes[1] = (integerToRegister(arg1));

                uint16_t num = static_cast<uint16_t>(arg2);

                ins.bytes[2] = ( (num & 0xFF00) >> 8 ) ;
                ins.bytes[3] = ( (num & 0x00FF) >> 0 ) ;

                uint16_t num1 = static_cast<uint16_t>(arg3);

                ins.bytes[4] = ( (num1 & 0xFF00) >> 8 ) ;
                ins.bytes[5] = ( (num1 & 0x00FF) >> 0 ) ;

                ins.bytes[6] = 0xFF;
                ins.bytes[7] = 0xFF;
                break; 
            }

            default:      
                std::cerr << "SOMETHING AWFUL HAPPENED DON'T TRUST ANYTHING" << std::endl;
                return ins; // Keep that compiler happy.
        }

        // dumpInstruction(ins);
        return ins;
    }

    // ------------------------------------------------------------------------
    // 
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createNopInstruction()
    {
        Instruction ins;

        ins.bytes[0] = NABLA::VSYS::INS_NOP;
        ins.bytes[1] = 0xFF;
        ins.bytes[2] = 0xFF;
        ins.bytes[3] = 0xFF;
        ins.bytes[4] = 0xFF;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;

        return ins;
    }

    // ------------------------------------------------------------------------
    // 
    // ------------------------------------------------------------------------

    Bytegen::Instruction Bytegen::createSizeInstruction(uint8_t reg, Stacks stack)
    {
        Instruction ins;

        ins.bytes[0] = NABLA::VSYS::INS_SIZE;
        ins.bytes[1] = integerToRegister(reg);
        ins.bytes[2] = getStackAddress(stack);
        ins.bytes[3] = 0xFF;
        ins.bytes[4] = 0xFF;
        ins.bytes[5] = 0xFF;
        ins.bytes[6] = 0xFF;
        ins.bytes[7] = 0xFF;

        return ins;
    }

} // End of namespace
