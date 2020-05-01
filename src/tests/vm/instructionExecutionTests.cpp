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

namespace
{
    typedef struct VM * NablaVirtualMachine;

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    std::vector<uint8_t> ins_to_vec(NABLA::Bytegen::Instruction ins)
    {
        return std::vector<uint8_t>(std::begin(ins.bytes), std::end(ins.bytes));
    }
        
    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    uint16_t getRandom16(uint16_t low, uint16_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        return dis(gen);
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    double getRandomDouble(uint16_t low, uint16_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dis(low, high);
        return dis(gen);
    }
    
    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    bool check_double_equal(double lhs, double rhs)
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

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    bool check_result(NablaVirtualMachine vm, int16_t dest_reg, uint64_t expected)
    {
        return ((int64_t)vm->registers[dest_reg] == (int64_t)expected);
    }

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------
    
    uint64_t doubleToUint64(double val)
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
    
    double uint64ToDouble(uint64_t val)
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
    
    uint64_t calculateArith( NABLA::Bytegen::ArithmaticTypes arithType, uint64_t lhs, uint64_t rhs)
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

    uint64_t calculateBitwise( NABLA::Bytegen::BitwiseTypes type, uint64_t lhs, uint64_t rhs)
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

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

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

            int16_t dest_reg    = getRandom16(0, 9);
            uint64_t arg1;
            uint64_t arg2;

            uint64_t expectedResult;

            switch(arithSetup)
            {
                case NABLA::Bytegen::ArithmaticSetup::REG_REG: 
                    arg1 = getRandom16(0, 9); vm->registers[arg1] = getRandom16(0, 65000); // random reg with random val
                    arg2 = getRandom16(0, 9); vm->registers[arg2] = getRandom16(0, 65000); // random reg with random val

                    expectedResult = calculateArith(arithType, vm->registers[arg1], vm->registers[arg2]);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::REG_NUM: 
                    arg1 = getRandom16(0, 9); vm->registers[arg1] = getRandom16(0, 65000); // Random reg with random val
                    arg2 = getRandom16(0, 65000);                                            // Random val

                    expectedResult = calculateArith(arithType, vm->registers[arg1], arg2);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::NUM_REG: 
                    arg2 = getRandom16(0, 9); vm->registers[arg2] = getRandom16(0, 65000); // Random reg with random val
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

        // Registers for comparisons will be 1-9
        uint16_t reg1 = getRandom16(1,9);
        uint16_t reg2 = getRandom16(1,9);

        while(reg1 == reg2)
        {
            reg2 = getRandom16(1,9);
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

        // Registers for comparisons will be 1-9
        uint16_t reg1 = getRandom16(1,9);
        uint16_t reg2 = getRandom16(1,9);

        while(reg1 == reg2)
        {
            reg2 = getRandom16(1,9);
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

        // Step 2 instruction (should be branch check, but no add)
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
    // Each of the arithmatic types (ADD ,MUL, DIV, SUB)
    for(int typesItr = 0x04; typesItr <= 0x07; typesItr++)
    {
        NABLA::Bytegen::ArithmaticTypes arithType = static_cast<NABLA::Bytegen::ArithmaticTypes>(typesItr);

        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        // The only valid double arith
        NABLA::Bytegen::ArithmaticSetup arithSetup = NABLA::Bytegen::ArithmaticSetup::REG_REG;

        int16_t dest_reg    = getRandom16(0, 9);
        uint64_t arg1;
        uint64_t arg2;

        uint64_t expectedResult;

        switch(arithSetup)
        {
            case NABLA::Bytegen::ArithmaticSetup::REG_REG: 
                arg1 = getRandom16(0, 9); vm->registers[arg1] = doubleToUint64(getRandomDouble(0.0, 65000.0)); // random reg with random val
                arg2 = getRandom16(0, 9); vm->registers[arg2] = doubleToUint64(getRandomDouble(0.0, 65000.0)); // random reg with random val

                expectedResult = calculateArith(arithType, vm->registers[arg1], vm->registers[arg2]);
                break;

            case NABLA::Bytegen::ArithmaticSetup::REG_NUM: 
                arg1 = getRandom16(0, 9); vm->registers[arg1] =doubleToUint64(getRandomDouble(0.0, 65000.0)); // Random reg with random val
                arg2 = doubleToUint64(getRandomDouble(0.0, 65000.0));                                          // Random val

                expectedResult = calculateArith(arithType, vm->registers[arg1], arg2);
                break;

            case NABLA::Bytegen::ArithmaticSetup::NUM_REG: 
                arg2 = getRandom16(0, 9); vm->registers[arg2] = doubleToUint64(getRandomDouble(0.0, 65000.0)); // Random reg with random val
                arg1 = doubleToUint64(getRandomDouble(0.0, 65000.0));                                           // Random val

                expectedResult = calculateArith(arithType, arg1, vm->registers[arg2]);
                break;

            case NABLA::Bytegen::ArithmaticSetup::NUM_NUM: 
                arg1 = doubleToUint64(getRandomDouble(0.0, 65000.0));                                            // Random val
                arg2 = doubleToUint64(getRandomDouble(0.0, 65000.0));                                            // Random val

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

        CHECK_TRUE(check_double_equal(uint64ToDouble(vm->registers[dest_reg]), uint64ToDouble(expectedResult)));

        vm_delete(vm);
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, doubleBranchIns)
{
    for(int i = 0x07; i <= 0x0C; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-9
        uint16_t reg1 = getRandom16(1,9);
        uint16_t reg2 = getRandom16(1,9);

        while(reg1 == reg2)
        {
            reg2 = getRandom16(1,9);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGTD)
        {
            vm->registers[reg1] = doubleToUint64(getRandomDouble(500, 600));
            vm->registers[reg2] = doubleToUint64(getRandomDouble(0, 200));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTED)
        {
            vm->registers[reg1] = doubleToUint64(10.0);
            vm->registers[reg2] = doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTD)
        {
            vm->registers[reg1] = doubleToUint64(getRandomDouble(0, 200));
            vm->registers[reg2] = doubleToUint64(getRandomDouble(500, 600));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTED)
        {
            vm->registers[reg1] = doubleToUint64(10.0);
            vm->registers[reg2] = doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BEQD)
        {
            vm->registers[reg1] = doubleToUint64(10.0);
            vm->registers[reg2] = doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BNED)
        {
            vm->registers[reg1] = doubleToUint64(10.0);
            vm->registers[reg2] = doubleToUint64(25.0);
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

TEST(NablaInstructionTests, doubleBranchInsFails)
{
    for(int i = 0x07; i <= 0x0C; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-9
        uint16_t reg1 = getRandom16(1,9);
        uint16_t reg2 = getRandom16(1,9);

        while(reg1 == reg2)
        {
            reg2 = getRandom16(1,9);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGTD)
        {
            vm->registers[reg1] = doubleToUint64(getRandomDouble(0, 200));
            vm->registers[reg2] = doubleToUint64(getRandomDouble(500, 600));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTED)
        {
            vm->registers[reg1] = doubleToUint64(9.0);
            vm->registers[reg2] = doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTD)
        {
            vm->registers[reg1] = doubleToUint64(getRandomDouble(500, 700));
            vm->registers[reg2] = doubleToUint64(getRandomDouble(100, 200));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTED)
        {
            vm->registers[reg1] = doubleToUint64(15.0);
            vm->registers[reg2] = doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BEQD)
        {
            vm->registers[reg1] = doubleToUint64(55.0);
            vm->registers[reg2] = doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BNED)
        {
            vm->registers[reg1] = doubleToUint64(25.0);
            vm->registers[reg2] = doubleToUint64(25.0);
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

        // Step 2 instruction (should be branch check that fails)
        vm_step(vm, 2);

        CHECK_EQUAL(1, vm->registers[0]);

        // If the previous test passes. Then we are complete. To avoid anything crazy, we kill the vm
        vm_delete(vm);
    }
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

        uint16_t pushReg = getRandom16(0, 9);
        uint16_t popReg  = getRandom16(0, 9);

        // Ensure push and pop reg differ
        while(pushReg == popReg)
        {
            popReg = getRandom16(0, 9);
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

TEST(NablaInstructionTests, jumpInsLow)
{
    // Jump low
    for(int i = 0; i < 5; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        vm->registers[0]    = 0;

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        NABLA::Bytegen::Instruction jmpIns = bytegen.createJumpInstruction(
            0
        );

        build_test_vm(vm, ins_to_vec(baseIns));
        build_test_vm(vm, ins_to_vec(jmpIns));

        // Init
        vm_init(vm);

        // Step 1 instruction (should be add)
        vm_step(vm, 1);

        // Sanity
        CHECK_EQUAL(1, vm->registers[0]);

        // Step 2 instruction (should be jump, followed by add)
        vm_step(vm, 2);

        CHECK_EQUAL(2, vm->registers[0]);

        // If the previous test passes. Then we are complete. To avoid anything crazy, we kill the vm
        vm_delete(vm);
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, jumpInsHigh)
{
    // Jump high
    for(int i = 0; i < 5; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        vm->registers[0]    = 0;

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );
        
        vm->registers[9]    = 0;

        NABLA::Bytegen::Instruction baseIns1 = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            9,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        NABLA::Bytegen::Instruction jmpIns = bytegen.createJumpInstruction(
            2 // Jump over baseIns to baseIns2
        );

        build_test_vm(vm, ins_to_vec(jmpIns));
        build_test_vm(vm, ins_to_vec(baseIns)); // This one is jumped over
        build_test_vm(vm, ins_to_vec(baseIns1));

        // Init
        vm_init(vm);

        // Step 1 instruction (should be jump)
        vm_step(vm, 1);

        // Sanity
        CHECK_EQUAL(0, vm->registers[0]);

        // Step 2 instruction (should be jump, followed by add to register 2)
        vm_step(vm, 2);

        // Sanity
        CHECK_EQUAL(0, vm->registers[0]);   // Hopefully skipped

        CHECK_EQUAL(1, vm->registers[9]);  // Hopefully added 2

        // If the previous test passes. Then we are complete. To avoid anything crazy, we kill the vm
        vm_delete(vm);
    }
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

        int16_t dest_reg    = getRandom16(0, 9);

        uint64_t arg1;
        uint64_t expectedResult;

        if(setup == NABLA::Bytegen::MovSetup::REG_REG)
        {
            arg1 = getRandom16(0, 9); 
            expectedResult = getRandom16(0, 254);
            vm->registers[arg1] = expectedResult;
        }
        else
        {
            // REG_NUM needs to ensure int8_t as the numerical encoding is limited to int8_t
            expectedResult =  (int8_t)getRandom16(0, 254);
            arg1 = expectedResult;
        } 

        NABLA::Bytegen::Instruction ins = bytegen.createMovInstruction(
            setup,
            dest_reg,
            arg1
        );

        build_test_vm(vm, ins_to_vec(ins));

        vm_run(vm);

        std::cout << "DEST REG: " << dest_reg << " | Expected : " << expectedResult << " | actual : " << vm->registers[dest_reg] << std::endl;

        CHECK_TRUE(check_result(vm, dest_reg, expectedResult));

        vm_delete(vm);
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, stbLdbIns)
{
    for(int i = 0; i < 10; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::Stacks stackLoc = static_cast<NABLA::Bytegen::Stacks>(getRandom16(0,1));

        uint16_t reg = getRandom16(0, 8);

        vm->registers[reg] = getRandom16(0, 65530);

        NABLA::Bytegen::Instruction storeIns;
        NABLA::Bytegen::Instruction loadIns;

        // Randomly decide if we should use register based, or number based store / load instructions
        if(getRandom16(0, 1) == 0)
        {
            // Number-based store instruction
            storeIns = bytegen.createStbInstruction(
                stackLoc, NABLA::Bytegen::LoadStoreSetup::NUMBER_BASED, i, reg
            );

            loadIns = bytegen.createLdbInstruction(
                stackLoc, NABLA::Bytegen::LoadStoreSetup::NUMBER_BASED, i, reg+1
            );
        }
        else
        {
            // Register-based store instruction

            uint16_t sourceReg = getRandom16(0, 8);

            while(reg == sourceReg)
            {
                sourceReg = getRandom16(0, 8);
            }
            
            vm->registers[sourceReg] = i;

            storeIns = bytegen.createStbInstruction(
                stackLoc, NABLA::Bytegen::LoadStoreSetup::REGISTER_BASED, sourceReg, reg
            );

            loadIns = bytegen.createLdbInstruction(
                stackLoc, NABLA::Bytegen::LoadStoreSetup::REGISTER_BASED, sourceReg, reg+1
            );
        }

        /*
            No matter where we are getting the information for store / load (register vs numerical constant)
            the actual test is the same as we've populate a a source register if we've needed, and checking it
            isn't necissary
        */

        // Get the bytes for the instruction
        std::vector<uint8_t> storeBytes = ins_to_vec(storeIns);
        std::vector<uint8_t> loadBytes  = ins_to_vec(loadIns);

        // Populate vm
        build_test_vm(vm, storeBytes);
        build_test_vm(vm, loadBytes);

        // Init
        vm_init(vm);

        // Extend the stacks so we can put and get from them
        for(int p = 0; p < 50; p++)
        {
            int pushResult;
            stack_push(0, vm->globalStack, &pushResult);
            stack_push(0, vm->functions[0].localStack, &pushResult);

            CHECK_EQUAL(pushResult, STACK_OKAY);
        }

        // Step 1 instruction (store)
        vm_step(vm, 1);

        // Depending on the stack, get the value that should have been pushed
        int64_t val;
        int result = 0;
        if(stackLoc == NABLA::Bytegen::Stacks::GLOBAL)
        {
            val = stack_value_at(i, vm->globalStack, &result);
        }
        else
        {
            val = stack_value_at(i, vm->functions[0].localStack, &result);
        }

        // Ensure stack grab was okay. and ensure the value retrieved was the value we put in
        CHECK_EQUAL(result, STACK_OKAY);
        CHECK_EQUAL(vm->registers[reg], val);

        // Step again to execute load
        vm_step(vm, 1);

        // See if correct val is loaded
        CHECK_EQUAL(val, vm->registers[reg+1]);

        vm_delete(vm);
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, callReturnIns)
{
    NABLA::Bytegen bytegen;
    
    NablaVirtualMachine vm = vm_new();

    vm->fp = 0;
    vm->entryAddress = 0;
    vm->numberOfFunctions = 2;

    std::vector<NABLA::Bytegen::Instruction> callIns = bytegen.createCallInstruction(
        0, // Function from  
        1, // Return-to instruction
        1  // Function to call
    );

    for(auto &ei : callIns)
    {
        std::vector<uint8_t> callBytes = ins_to_vec(ei);

        for(uint64_t ins = 0; ins < callBytes.size()/8; ins++)
        {
            uint64_t currentIns = 0;
            for(int n = 7; n >= 0; n--)   
            {
                currentIns |= (uint64_t)callBytes[ins++] << (n * 8);
            }

            int pushResult;
            stack_push(currentIns, vm->functions[0].instructions, &pushResult);

            if(pushResult != STACK_OKAY)
            {
                FAIL("Failed to push instruction to vm");
            }
        }
    }

    // --------------------------------------------------------------------

    NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
        NABLA::Bytegen::ArithmaticTypes::ADD,
        NABLA::Bytegen::ArithmaticSetup::REG_NUM,
        0,  // Destination register
        0,  // reg 0
        1   // Inc reg1 by 1 every time this instruction is called
    );

    std::vector<uint8_t> instructions = ins_to_vec(baseIns);

    for(uint64_t ins = 0; ins < instructions.size()/8; ins++)
    {
        uint64_t currentIns = 0;
        for(int n = 7; n >= 0; n--)   
        {
            currentIns |= (uint64_t)instructions[ins++] << (n * 8);
        }

        int pushResult;
        stack_push(currentIns, vm->functions[1].instructions, &pushResult);

        if(pushResult != STACK_OKAY)
        {
            FAIL("Failed to push instruction to vm");
        }

    }

    // --------------------------------------------------------------------

    // Init
    vm_init(vm);

    // Step again to execute load
    vm_step(vm, 3);

    // Ensure we have entered new 'called' function
    CHECK_EQUAL(1, vm->fp)

    vm_step(vm, 3);
    
    // See if correct val is loaded by the function that was called
    CHECK_EQUAL(1, vm->registers[0]);

    // Ensure fp came back to 0
    CHECK_EQUAL(0, vm->fp)

    vm_delete(vm);
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, exitIns)
{
    NABLA::Bytegen bytegen;
    NablaVirtualMachine vm = vm_new();

    vm->registers[0]    = 0;

    NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
        NABLA::Bytegen::ArithmaticTypes::ADD,
        NABLA::Bytegen::ArithmaticSetup::REG_NUM,
        0,  // Destination register
        0,  // reg 0
        1   // Inc reg1 by 1 every time this instruction is called
    );

    NABLA::Bytegen::Instruction exitIns = bytegen.createExitInstruction();

    NABLA::Bytegen::Instruction baseIns1 = bytegen.createArithmaticInstruction(
        NABLA::Bytegen::ArithmaticTypes::ADD,
        NABLA::Bytegen::ArithmaticSetup::REG_NUM,
        0,  // Destination register
        0,  // reg 0
        1   // Inc reg1 by 1 every time this instruction is called
    );

    build_test_vm(vm, ins_to_vec(baseIns));
    build_test_vm(vm, ins_to_vec(exitIns));
    build_test_vm(vm, ins_to_vec(baseIns1));

    // Init
    vm_init(vm);

    // Step 1 instruction (should be add)
    vm_step(vm, 1);

    // Sanity
    CHECK_EQUAL(1, vm->registers[0]);

    // Step a bunch and see if we ever trigger the 
    // next add instruction (shouldnt be reachable due to exit)
    vm_step(vm, 10);

    CHECK_EQUAL(1, vm->registers[0]);

    // If the previous test passes. Then we are complete.
    vm_delete(vm);
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, nopIns)
{
    for(int t = 0; t < 100; t++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        vm->registers[0]    = 0;

        uint16_t randomNumAdds = getRandom16(1, 500);

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        // Add a random number of additions
        for(int i = 0; i < randomNumAdds; i++)
        {
            build_test_vm(vm, ins_to_vec(baseIns));
        }

        NABLA::Bytegen::Instruction nopIns = bytegen.createNopInstruction();

        uint16_t randomNumNops = getRandom16(1, 500);

        // Add a random number of additions
        for(int i = 0; i < randomNumNops; i++)
        {
            build_test_vm(vm, ins_to_vec(nopIns));
        }

        vm_init(vm);

        // Step over the adds
        vm_step(vm, randomNumAdds);

        // Ensure that all of the adds have occured
        CHECK_EQUAL(randomNumAdds, vm->registers[0]);

        // Step the noops
        vm_step(vm, randomNumNops);

        // Ensure the addition sum is the same as before noops
        CHECK_EQUAL(randomNumAdds, vm->registers[0]);

        // Ensure that the instruction pointer is equal to all of the instructions executed
        CHECK_EQUAL(randomNumAdds + randomNumNops, vm->functions[vm->fp].ip);

        vm_delete(vm);
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaInstructionTests, bitwiseIns)
{
    // Each of the arithmatic types (ADD ,MUL, DIV, SUB)
    for(int typesItr = 0x01; typesItr <= 0x06; typesItr++)
    {
        NABLA::Bytegen::BitwiseTypes arithType = static_cast<NABLA::Bytegen::BitwiseTypes>(typesItr);

        for(int setupItr = 0; setupItr <= 3; setupItr++)
        {
            NABLA::Bytegen::ArithmaticSetup arithSetup = static_cast<NABLA::Bytegen::ArithmaticSetup>(setupItr);

            // NOT instruction only have 2 seperate arithmatic setups, so we skip the invalids
            if(NABLA::Bytegen::BitwiseTypes::NOT == arithType)
            {
                if(NABLA::Bytegen::ArithmaticSetup::REG_NUM == arithSetup || 
                   NABLA::Bytegen::ArithmaticSetup::NUM_NUM == arithSetup)
                {
                    continue;
                }
            }

            NABLA::Bytegen bytegen;
            NablaVirtualMachine vm = vm_new();


            int16_t dest_reg    = getRandom16(0, 9);
            uint64_t arg1;
            uint64_t arg2;

            uint64_t expectedResult;

            switch(arithSetup)
            {
                case NABLA::Bytegen::ArithmaticSetup::REG_REG: 
                    arg1 = getRandom16(0, 9); vm->registers[arg1] = getRandom16(0, 65000); // random reg with random val
                    arg2 = getRandom16(0, 9); vm->registers[arg2] = getRandom16(0, 65000); // random reg with random val

                    expectedResult = calculateBitwise(arithType, vm->registers[arg1], vm->registers[arg2]);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::REG_NUM: 
                    arg1 = getRandom16(0, 9); vm->registers[arg1] = getRandom16(0, 65000); // Random reg with random val
                    arg2 = getRandom16(0, 65000);                                            // Random val

                    expectedResult = calculateBitwise(arithType, vm->registers[arg1], arg2);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::NUM_REG: 
                    arg2 = getRandom16(0, 9); vm->registers[arg2] = getRandom16(0, 65000); // Random reg with random val
                    arg1 = getRandom16(0, 65000);                                           // Random val

                    expectedResult = calculateBitwise(arithType, arg1, vm->registers[arg2]);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::NUM_NUM: 
                    arg1 = getRandom16(0, 65000);                                            // Random val
                    arg2 = getRandom16(0, 65000);                                            // Random val

                    expectedResult = calculateBitwise(arithType, arg1, arg2);
                    break;
            }

            NABLA::Bytegen::ArithmaticSetup setup;

            // Add registers r0 and r1 together, place in destination
            NABLA::Bytegen::Instruction ins = bytegen.createBitwiseInstruction(
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

TEST(NablaInstructionTests, sizeInstruction)
{
    for(int i = 0; i < 100; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::Stacks stackLoc = static_cast<NABLA::Bytegen::Stacks>(getRandom16(0,1));

        // A register to get data from for push instructions used to grow the stack for testing
        uint16_t pushReg = getRandom16(0, 9);

        // Where the size instruction should be placing size once acquired
        uint16_t dest_reg = getRandom16(0, 9);

        // Ensure push and pop reg differ
        while(pushReg == dest_reg)
        {
            dest_reg = getRandom16(0, 9);
        }

        // Put some random value, we really don't care what it is
        vm->registers[pushReg] = getRandom16(0, 65530);

        // Create a size instruction that should read '0' when called 
        NABLA::Bytegen::Instruction sizeIns = bytegen.createSizeInstruction(dest_reg, stackLoc);

        std::vector<uint8_t> sizeInsBytes = ins_to_vec(sizeIns);
        build_test_vm(vm, sizeInsBytes);

        // Grow the stack some number of sizes
        uint8_t numPushs = getRandom16(0, 55);

        for(int sp = 0; sp < numPushs; sp++)
        {
            NABLA::Bytegen::Instruction pushIns = bytegen.createPushInstruction(
                stackLoc, pushReg
            );

            std::vector<uint8_t> pushBytes = ins_to_vec(pushIns);
            build_test_vm(vm, pushBytes);
        }

        // Create a stack instruction that should read back numPushs
        sizeInsBytes.clear();
        sizeInsBytes = ins_to_vec(bytegen.createSizeInstruction(dest_reg, stackLoc));
        build_test_vm(vm, sizeInsBytes);

        // Init
        vm_init(vm);

        // Step 1 instruction (should be push)
        vm_step(vm, 1);

        // Size should have read 0
        CHECK_EQUAL(0, vm->registers[dest_reg]);

        // Step all of the push instructions
        vm_step(vm, numPushs);
     
        // Sanity check
        CHECK_EQUAL(0, vm->registers[dest_reg]);
        
        // Execute the 2nd size instruction
        vm_step(vm, 1);

        // Make sure its reading a size of 'numPushs'
        CHECK_EQUAL(numPushs, vm->registers[dest_reg]);

        vm_delete(vm);
    }
}