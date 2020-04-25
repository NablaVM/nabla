/*
    These are tests for the NABLA VM, but they are placed here so we can leverage CPPUTEST. 

    For each group of instructions, we create a VM loaded with instructions. Tell it to execute, 
    and check the resulting calculations to ensure that instructions that the VM executes are doing 
    so as we expect.
*/

#include <iostream>
#include "bytegen.hpp"
#include <random>
#include <vector>
#include "CppUTest/TestHarness.h"

extern "C" 
{
    #include "VmCommon.h"
    #include <stdio.h>
    #include "vm.h"
    #include "VmInstructions.h"
    #include "stack.h"
}

namespace
{
    typedef struct VM * NablaVirtualMachine;

    std::vector<uint8_t> ins_to_vec(NABLA::Bytegen::Instruction ins)
    {
        return std::vector<uint8_t>(std::begin(ins.bytes), std::end(ins.bytes));
    }
        
    uint16_t getRandom16(uint16_t low, uint16_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        return dis(gen);
    }

    void build_test_vm(NablaVirtualMachine vm, std::vector<uint8_t> instructions)
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

    bool check_result(NablaVirtualMachine vm, int16_t dest_reg, uint64_t expected)
    {
        return ((int64_t)vm->registers[dest_reg] == (int64_t)expected);
    }

    uint64_t calculateArith( NABLA::Bytegen::ArithmaticTypes arithType, uint64_t lhs, uint16_t rhs)
    {
        switch(arithType)
        {
            case NABLA::Bytegen::ArithmaticTypes::ADD:  return lhs + rhs;
            case NABLA::Bytegen::ArithmaticTypes::SUB:  return lhs - rhs;
            case NABLA::Bytegen::ArithmaticTypes::DIV:  return lhs / rhs;
            case NABLA::Bytegen::ArithmaticTypes::MUL:  return lhs * rhs;
            default: return 0;
        };
        return 0;
    }
}


TEST_GROUP(NablaInstructionTests)
{
    void setup()
    {
        MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    }

    void teardown()
    {
        MemoryLeakWarningPlugin::turnOnNewDeleteOverloads();
    }
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, standardArith)
{
    // Each of the arithmatic types (ADD ,MUL, DIV, SUB)
    for(int typesItr = 0x00; typesItr <= 0x03; typesItr++)
    {
        NABLA::Bytegen::ArithmaticTypes arithType = static_cast<NABLA::Bytegen::ArithmaticTypes>(typesItr);

        for(int setupItr = 0; setupItr <= 3; setupItr++)
        {
            NABLA::Bytegen bytegen;
            NablaVirtualMachine vm = vm_new();

            NABLA::Bytegen::ArithmaticSetup arithSetup = static_cast<NABLA::Bytegen::ArithmaticSetup>(setupItr);

            int16_t dest_reg    = getRandom16(0, 15);
            uint64_t arg1;
            uint64_t arg2;

            uint64_t expectedResult;

            switch(arithSetup)
            {
                case NABLA::Bytegen::ArithmaticSetup::REG_REG: 
                    arg1 = getRandom16(0, 15); vm->registers[arg1] = getRandom16(0, 65000); // random reg with random val
                    arg2 = getRandom16(0, 15); vm->registers[arg2] = getRandom16(0, 65000); // random reg with random val

                    expectedResult = calculateArith(arithType, vm->registers[arg1], vm->registers[arg2]);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::REG_NUM: 
                    arg1 = getRandom16(0, 15); vm->registers[arg1] = getRandom16(0, 65000); // Random reg with random val
                    arg2 = getRandom16(0, 65000);                                            // Random val

                    expectedResult = calculateArith(arithType, vm->registers[arg1], arg2);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::NUM_REG: 
                    arg2 = getRandom16(0, 15); vm->registers[arg2] = getRandom16(0, 65000); // Random reg with random val
                    arg1 = getRandom16(0, 65000);                                           // Random val

                    expectedResult = calculateArith(arithType, arg1, vm->registers[arg2]);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::NUM_NUM: 
                    arg1 = getRandom16(0, 65000);                                            // Random val
                    arg2 = getRandom16(0, 65000);                                            // Random val

                    expectedResult = calculateArith(arithType, arg1, arg2);
                    break;
            }

            NABLA::Bytegen::ArithmaticSetup setup;

            // Add registers r0 and r1 together, place in destination
            NABLA::Bytegen::Instruction ins = bytegen.createArithmaticInstruction(
                arithType,
                arithSetup,
                dest_reg,
                arg1,
                arg2
            );

            build_test_vm(vm, ins_to_vec(ins));

            vm_run(vm);

            CHECK_TRUE(check_result(vm, dest_reg, expectedResult));

            vm_delete(vm);
        }
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, branchIns)
{
    std::cout << "(NablaInstructionTests, branchIns)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, pushPopIns)
{
    std::cout << "(NablaInstructionTests, pushPopIns)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, jumpIns)
{
    std::cout << "(NablaInstructionTests, jumpIns)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, movIns)
{
    std::cout << "(NablaInstructionTests, movIns)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, stbLdbIns)
{
    std::cout << "(NablaInstructionTests, stbLdbIns)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, callIns)
{
    std::cout << "(NablaInstructionTests, callIns)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, returnIns)
{
    std::cout << "(NablaInstructionTests, returnIns)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, exitIns)
{
    std::cout << "(NablaInstructionTests, exitIns)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, bitwiseIns)
{
    std::cout << "(NablaInstructionTests, bitwiseIns)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, nopIns)
{
    std::cout << "(NablaInstructionTests, nopIns)\t\t This test needs to be written here" << std::endl;
}