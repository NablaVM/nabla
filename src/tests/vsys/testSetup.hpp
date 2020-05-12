#ifndef NABLA_VM_EXEC_TESTS_HPP
#define NABLA_VM_EXEC_TESTS_HPP

#include <iostream>
#include "bytegen.hpp"
#include <random>
#include <vector>
#include <ieee754.h>

#include "CppUTest/TestHarness.h"

#include "VSysMachine.hpp"
#include "VSysExecutionContext.hpp"
#include "VSysExecutionReturns.hpp"

namespace TEST
{
    class TestMachine : public NABLA::VSYS::Machine
    {
    public:
        TestMachine()
        {    
            // Ensure execution contexts arent wiped
            this->running = true;
            
            // Manually set a context
            this->executionContexts.push_back(
                NABLA::VSYS::ExecutionContext(*this, 0, this->global_memory, this->functions)
            );
        }

        ~TestMachine(){}

        void build (std::vector<uint8_t> instructions)
        {
            this->entryAddress = 0;
            this->numberOfFunctions = 1;

            std::vector<uint64_t> func;

            // Read all of the instructions from the file
            for(uint64_t ins = 0; ins < instructions.size()/8; ins++)
            {
                uint64_t currentIns = 0;
                for(int n = 7; n >= 0; n--)   
                {
                    currentIns |= (uint64_t)instructions[ins++] << (n * 8);
                }

                func.push_back(currentIns);
            }

            this->functions.push_back(func);

        }

        void setReg(uint8_t regNum, uint64_t val)
        {
            this->executionContexts[0].registers[regNum] = val;
        }

        uint64_t getReg(uint8_t regNum)
        {
            return this->executionContexts[0].registers[regNum];
        }
    };

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
    
    static bool check_result(TestMachine vm, int16_t dest_reg, uint64_t expected)
    {
        return ((int64_t)vm.getReg(dest_reg) == (int64_t)expected);
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

