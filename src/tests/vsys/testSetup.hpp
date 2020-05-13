#ifndef NABLA_VM_EXEC_TESTS_HPP
#define NABLA_VM_EXEC_TESTS_HPP

#include <assert.h>
#include <iostream>
#include "bytegen.hpp"
#include <random>
#include <vector>
#include <ieee754.h>


#include "VSysMachine.hpp"
#include "VSysMemory.hpp"
#include "VSysExecutionContext.hpp"
#include "VSysExecutionReturns.hpp"
#include <vector>

// Always include this last
#include "CppUTest/TestHarness.h"

namespace TEST
{
    /*
    
            An execution context with some extra functions for setting up tests
    
    */
    class TestContext : public NABLA::VSYS::ExecutionContext
    {
    public:
        TestContext(NABLA::VSYS::Machine &owner, 
                    uint64_t startAddress, 
                    NABLA::VSYS::Memory<NABLA::VSYS::NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory, 
                    std::vector< std::vector<uint64_t> > &functions) : 
                    ExecutionContext(owner, startAddress, global_memory, functions)
        {
            for(int i = 0; i < 16; i++)
            {
                registers[i] = i;
            }
        }

        void resetFunctions(std::vector< std::vector<uint64_t> > &functions)
        {
            this->contextFunctions.clear();

            for(uint64_t i = 0; i < functions.size(); i++)
            {
                InstructionBlock ib;
                ib.instruction_pointer = 0;
                ib.instructions = &functions[i];
                contextFunctions.push_back(
                    ib
                );
            }
        }

        void dumpReg()
        {
            for(int i = 0; i < 16; i++)
            {
                std::cout << "REG: " << i << " " << registers[i] << std::endl;
            }
        }

    };

    /*
    
            A virtual machine with methods for building tests and accessing execution contexts
    
    */
    class TestMachine : public NABLA::VSYS::Machine
    {
    public:
        TestMachine() : test_context(*this, 0, this->global_memory, this->functions)
        {    
            // Ensure execution contexts arent wiped
            this->running = true;
        }

        ~TestMachine(){}

        void build (std::vector<uint8_t> instructions)
        {
            this->entryAddress = 0;
            this->numberOfFunctions = 1;

            std::vector<uint64_t> func;

            // Read all of the instructions 
            uint64_t s = 0;
            for(uint64_t ins = 0; ins < instructions.size()/8; ins++)
            {
                uint64_t currentIns = 0;
                for(int n = 7; n >= 0; n--)   
                {
                    currentIns |= (uint64_t)instructions[s++] << (n * 8);
                }

                func.push_back(currentIns);
            }

            this->functions.push_back(func);

            this->executionContexts.clear();
            
            test_context.resetFunctions(this->functions);

            // Manually set a context
            this->executionContexts.push_back(
                test_context
            );
        }

        void setReg(uint8_t regNum, uint64_t val)
        {
            this->test_context.registers[regNum] = val;
        }

        uint64_t getReg(uint8_t regNum)
        {
            return this->test_context.registers[regNum];
        }

        //  - Should only be called post-build and post-execution
        //
        uint64_t getActiveReg(uint8_t regNum)
        {
            return this->executionContexts[0].registers[regNum];
        }

        uint64_t getGlobalWord(uint64_t addr)
        {
            uint64_t v = 0;
            this->global_memory.get_64(addr, v);
            return v;
        }

        uint8_t getGlobalByte(uint64_t addr)
        {
            uint8_t v = 0;
            assert(this->global_memory.get_8(addr, v));
            return v;
        }

    private:
        TestContext test_context;
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
        return ((int64_t)vm.getActiveReg(dest_reg) == (int64_t)expected);
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

