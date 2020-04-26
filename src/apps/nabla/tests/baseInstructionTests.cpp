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

TEST(NablaInstructionTests, standardBranchIns)
{
    for(int i = 0x01; i <= 0x06; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-15
        uint16_t reg1 = getRandom16(1,15);
        uint16_t reg2 = getRandom16(1,15);

        while(reg1 == reg2)
        {
            reg2 = getRandom16(1,15);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGT)
        {
            vm->registers[reg1] = getRandom16(500, 600);
            vm->registers[reg2] = getRandom16(0, 200);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTE)
        {
            vm->registers[reg1] = 10;
            vm->registers[reg2] = 10;
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLT)
        {
            vm->registers[reg1] = getRandom16(0, 200);
            vm->registers[reg2] = getRandom16(500, 600);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTE)
        {
            vm->registers[reg1] = 10;
            vm->registers[reg2] = 10;
        }
        else if (type == NABLA::Bytegen::BranchTypes::BEQ)
        {
            vm->registers[reg1] = 10;
            vm->registers[reg2] = 10;
        }
        else if (type == NABLA::Bytegen::BranchTypes::BNE)
        {
            vm->registers[reg1] = 10;
            vm->registers[reg2] = 25;
        }

        // Instruction to branch to is at location 0, so ensure 0 is there to start. Should be 1 on first cycle
        vm->registers[0]    = 0;

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        NABLA::Bytegen::Instruction branchIns = bytegen.createBranchInstruction(
            type,   // Type of branch 
            reg1,   // Comparison reg 1
            reg2,   // Comparison reg 2
            0       // Location to branch to
        );

        // Populate vm
        build_test_vm(vm, ins_to_vec(baseIns));
        build_test_vm(vm, ins_to_vec(branchIns));

        // Init
        vm_init(vm);

        // Step 1 instruction (should be add)
        vm_step(vm, 1);

        // Sanity
        CHECK_EQUAL(1, vm->registers[0]);

        // Step 2 instruction (should be branch check, followed by add)
        vm_step(vm, 2);

        CHECK_EQUAL(2, vm->registers[0]);

        // If the previous test passes. Then we are complete. To avoid anything crazy, we kill the vm
        vm_delete(vm);
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, standardBranchInsExpectedFails)
{
    for(int i = 0x01; i <= 0x06; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-15
        uint16_t reg1 = getRandom16(1,15);
        uint16_t reg2 = getRandom16(1,15);

        while(reg1 == reg2)
        {
            reg2 = getRandom16(1,15);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGT)
        {
            vm->registers[reg1] = getRandom16(0, 200);
            vm->registers[reg2] = getRandom16(500, 600);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTE)
        {
            vm->registers[reg1] = 1;
            vm->registers[reg2] = 10;
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLT)
        {
            vm->registers[reg1] = getRandom16(500, 600); 
            vm->registers[reg2] = getRandom16(0, 200);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTE)
        {
            vm->registers[reg1] = 55;
            vm->registers[reg2] = 10;
        }
        else if (type == NABLA::Bytegen::BranchTypes::BEQ)
        {
            vm->registers[reg1] = 500;
            vm->registers[reg2] = 10;
        }
        else if (type == NABLA::Bytegen::BranchTypes::BNE)
        {
            vm->registers[reg1] = 25;
            vm->registers[reg2] = 25;
        }

        // Instruction to branch to is at location 0, so ensure 0 is there to start. Should be 1 on first cycle
        vm->registers[0]    = 0;

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        NABLA::Bytegen::Instruction branchIns = bytegen.createBranchInstruction(
            type,   // Type of branch 
            reg1,   // Comparison reg 1
            reg2,   // Comparison reg 2
            0       // Location to branch to
        );

        // Populate vm
        build_test_vm(vm, ins_to_vec(baseIns));
        build_test_vm(vm, ins_to_vec(branchIns));

        // Init
        vm_init(vm);

        // Step 1 instruction (should be add)
        vm_step(vm, 1);

        // Sanity
        CHECK_EQUAL(1, vm->registers[0]);

        // Step 2 instruction (should be branch check, followed by add)
        vm_step(vm, 2);

        CHECK_EQUAL(1, vm->registers[0]);

        // If the previous test passes. Then we are complete. To avoid anything crazy, we kill the vm
        vm_delete(vm);
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, doubleArith)
{
    std::cout << "(NablaInstructionTests, doubleArith)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, doubleBranchIns)
{
    std::cout << "(NablaInstructionTests, doubleBranchIns)\t This test needs to be written here" << std::endl;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, pushPopIns)
{
    for(int i = 0; i < 10; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::Stacks stackLoc = static_cast<NABLA::Bytegen::Stacks>(getRandom16(0,1));

        uint16_t pushReg = getRandom16(0,15);
        uint16_t popReg  = getRandom16(0,15);

        // Ensure push and pop reg differ
        while(pushReg == popReg)
        {
            popReg = getRandom16(0,15);
        }

        vm->registers[pushReg] = getRandom16(0, 65530);

        NABLA::Bytegen::Instruction pushIns = bytegen.createPushInstruction(
            stackLoc, pushReg
        );

        NABLA::Bytegen::Instruction popIns = bytegen.createPopInstruction(
            stackLoc, popReg
        );

        std::vector<uint8_t> pushBytes = ins_to_vec(pushIns);
        std::vector<uint8_t> popBytes  = ins_to_vec(popIns);

        // Populate vm
        build_test_vm(vm, pushBytes);
        build_test_vm(vm, popBytes);

        // Init
        vm_init(vm);

        // Step 1 instruction (should be push)
        vm_step(vm, 1);

        // Depending on the stack, get the value that should have been pushed
        int64_t val;
        int result = 0;
        if(stackLoc == NABLA::Bytegen::Stacks::GLOBAL)
        {
            val = stack_value_at(0, vm->globalStack, &result);
        }
        else
        {
            val = stack_value_at(0, vm->functions[0].localStack, &result);
        }

        // Ensure stack grab was okay. and ensure the value retrieved was the value we put in
        CHECK_EQUAL(result, STACK_OKAY);
        CHECK_EQUAL(vm->registers[pushReg], val);

        // Step again to execute pop
        vm_step(vm, 1);

        // See if correct val is stored
        CHECK_EQUAL(val, vm->registers[popReg]);

        vm_delete(vm);
    }
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
    for(int i = 0; i < 10; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::MovSetup setup = static_cast<NABLA::Bytegen::MovSetup>(getRandom16(0,1));

        int16_t dest_reg    = getRandom16(0, 15);

        uint64_t arg1;
        uint64_t expectedResult;

        if(setup == NABLA::Bytegen::MovSetup::REG_REG)
        {
            arg1 = getRandom16(0, 15); 
            expectedResult = getRandom16(0, 254);
            vm->registers[arg1] = expectedResult;
        }
        else
        {
            expectedResult =  getRandom16(0, 254);
            arg1 = expectedResult;
        } 

        NABLA::Bytegen::Instruction ins = bytegen.createMovInstruction(
            setup,
            dest_reg,
            arg1
        );

        build_test_vm(vm, ins_to_vec(ins));

        vm_run(vm);

        CHECK_TRUE(check_result(vm, dest_reg, expectedResult));

        vm_delete(vm);
    }
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