#ifndef NABLA_VM_EXEC_TESTS_HPP
#define NABLA_VM_EXEC_TESTS_HPP

#include <iostream>
#include "bytegen.hpp"
#include <random>
#include <vector>
#include <ieee754.h>

#include "CppUTest/TestHarness.h"
extern "C" 
{
    #include <stdio.h>
    #include "vm.h"
    #include "VmInstructions.h"
    #include "stack.h"
}

namespace TEST
{
    typedef struct VM * NablaVirtualMachine;

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    static std::vector<uint8_t> ins_to_vec(NABLA::Bytegen::Instruction ins)
    {
        return std::vector<uint8_t>(std::begin(ins.bytes), std::end(ins.bytes));
    }
        
    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    static uint16_t getRandomU16(uint16_t low, uint16_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        return dis(gen);
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    static int16_t getRandomS16(int16_t low, int16_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        return dis(gen);
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    static double getRandomDouble(uint16_t low, uint16_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(low, high);
        return dis(gen);
    }
    
    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    static bool check_double_equal(double lhs, double rhs)
    {
        double precision = 0.00001;
        if (((lhs - precision) < rhs) && 
            ((lhs + precision) > rhs))
        {
            return true;
        }
        return false;
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    static void build_test_vm(NablaVirtualMachine vm, std::vector<uint8_t> instructions)
    {
        vm->fp = 0;
        vm->entryAddress = 0;
        vm->numberOfFunctions = 1;
        // Read all of the instructions from the file
        for(uint64_t ins = 0; ins < instructions.size()/8; ins++)
        {
            uint64_t currentIns = 0;
            for(int n = 7; n >= 0; n--)   
            {
                currentIns |= (uint64_t)instructions[ins++] << (n * 8);
            }

            int pushResult;
            stack_push(currentIns, vm->functions[vm->fp].instructions, &pushResult);

            if(pushResult != STACK_OKAY)
            {
                FAIL("Failed to push instruction to vm");
            }
        }
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    static bool check_result(NablaVirtualMachine vm, int16_t dest_reg, uint64_t expected)
    {
        return ((int64_t)vm->registers[dest_reg] == (int64_t)expected);
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    static uint64_t doubleToUint64(double val)
    {
        ieee754_double ied;
        ied.d = val;
        uint64_t packed = (uint64_t)ied.ieee.negative  << 63| 
                          (uint64_t)ied.ieee.exponent  << 52|
                          (uint64_t)ied.ieee.mantissa0 << 32|
                          (uint64_t)ied.ieee.mantissa1 << 0;
        return packed;
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    static double uint64ToDouble(uint64_t val)
    {
        union deval
        {
            uint64_t val;
            double d;
        };

        union deval d; d.val = val;

        // Return double
        return d.d;
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    static int64_t calculateArith( NABLA::Bytegen::ArithmaticTypes arithType, int64_t lhs, int64_t rhs)
    {
        switch(arithType)
        {
            case NABLA::Bytegen::ArithmaticTypes::ADD:  return lhs + rhs;
            case NABLA::Bytegen::ArithmaticTypes::SUB:  return lhs - rhs;
            case NABLA::Bytegen::ArithmaticTypes::DIV:  return lhs / rhs;
            case NABLA::Bytegen::ArithmaticTypes::MUL:  return lhs * rhs;
            case NABLA::Bytegen::ArithmaticTypes::ADDD:  return doubleToUint64((uint64ToDouble(lhs) + uint64ToDouble(rhs)));
            case NABLA::Bytegen::ArithmaticTypes::SUBD:  return doubleToUint64((uint64ToDouble(lhs) - uint64ToDouble(rhs)));
            case NABLA::Bytegen::ArithmaticTypes::DIVD:  return doubleToUint64((uint64ToDouble(lhs) / uint64ToDouble(rhs)));
            case NABLA::Bytegen::ArithmaticTypes::MULD:  return doubleToUint64((uint64ToDouble(lhs) * uint64ToDouble(rhs)));
            default: return 0;
        };
        return 0;
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    static int64_t calculateBitwise( NABLA::Bytegen::BitwiseTypes type, int64_t lhs, int64_t rhs)
    {
        switch(type)
        {
            case NABLA::Bytegen::BitwiseTypes::LSH:  return lhs << rhs;
            case NABLA::Bytegen::BitwiseTypes::RSH:  return lhs >> rhs;
            case NABLA::Bytegen::BitwiseTypes::AND:  return lhs &  rhs;
            case NABLA::Bytegen::BitwiseTypes::OR :  return lhs |  rhs;
            case NABLA::Bytegen::BitwiseTypes::XOR:  return lhs ^  rhs;
            case NABLA::Bytegen::BitwiseTypes::NOT:  return     ~  lhs;
            default: return 0;
        };
        return 0;
    }
}

#endif

