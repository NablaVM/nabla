#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaBranchTests)
{
    void setup()
    {
       // MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    }

    void teardown()
    {
     //   MemoryLeakWarningPlugin::turnOnNewDeleteOverloads();
    }
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaBranchTests, standardBranchIns)
{
    for(int i = 0x01; i <= 0x06; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-9
        uint16_t reg1 = TEST::getRandomU16(1,9);
        uint16_t reg2 = TEST::getRandomU16(1,9);

        while(reg1 == reg2)
        {
            reg2 = TEST::getRandomU16(1,9);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGT)
        {
            vm->registers[reg1] = TEST::getRandomU16(500, 600);
            vm->registers[reg2] = TEST::getRandomU16(0, 200);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTE)
        {
            vm->registers[reg1] = 10;
            vm->registers[reg2] = 10;
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLT)
        {
            vm->registers[reg1] = TEST::getRandomU16(0, 200);
            vm->registers[reg2] = TEST::getRandomU16(500, 600);
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
        TEST::build_test_vm(vm, TEST::ins_to_vec(baseIns));
        TEST::build_test_vm(vm, TEST::ins_to_vec(branchIns));

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

TEST(NablaBranchTests, standardBranchInsExpectedFails)
{
    for(int i = 0x01; i <= 0x06; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-9
        uint16_t reg1 = TEST::getRandomU16(1,9);
        uint16_t reg2 = TEST::getRandomU16(1,9);

        while(reg1 == reg2)
        {
            reg2 = TEST::getRandomU16(1,9);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGT)
        {
            vm->registers[reg1] = TEST::getRandomU16(0, 200);
            vm->registers[reg2] = TEST::getRandomU16(500, 600);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTE)
        {
            vm->registers[reg1] = 1;
            vm->registers[reg2] = 10;
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLT)
        {
            vm->registers[reg1] = TEST::getRandomU16(500, 600); 
            vm->registers[reg2] = TEST::getRandomU16(0, 200);
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
        TEST::build_test_vm(vm, TEST::ins_to_vec(baseIns));
        TEST::build_test_vm(vm, TEST::ins_to_vec(branchIns));

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

TEST(NablaBranchTests, doubleBranchIns)
{
    for(int i = 0x07; i <= 0x0C; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-9
        uint16_t reg1 = TEST::getRandomU16(1,9);
        uint16_t reg2 = TEST::getRandomU16(1,9);

        while(reg1 == reg2)
        {
            reg2 = TEST::getRandomU16(1,9);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGTD)
        {
            vm->registers[reg1] = TEST::doubleToUint64(TEST::getRandomDouble(500, 600));
            vm->registers[reg2] = TEST::doubleToUint64(TEST::getRandomDouble(0, 200));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTED)
        {
            vm->registers[reg1] = TEST::doubleToUint64(10.0);
            vm->registers[reg2] = TEST::doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTD)
        {
            vm->registers[reg1] = TEST::doubleToUint64(TEST::getRandomDouble(0, 200));
            vm->registers[reg2] = TEST::doubleToUint64(TEST::getRandomDouble(500, 600));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTED)
        {
            vm->registers[reg1] = TEST::doubleToUint64(10.0);
            vm->registers[reg2] = TEST::doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BEQD)
        {
            vm->registers[reg1] = TEST::doubleToUint64(10.0);
            vm->registers[reg2] = TEST::doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BNED)
        {
            vm->registers[reg1] = TEST::doubleToUint64(10.0);
            vm->registers[reg2] = TEST::doubleToUint64(25.0);
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
        TEST::build_test_vm(vm, TEST::ins_to_vec(baseIns));
        TEST::build_test_vm(vm, TEST::ins_to_vec(branchIns));

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

TEST(NablaBranchTests, doubleBranchInsFails)
{
    for(int i = 0x07; i <= 0x0C; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-9
        uint16_t reg1 = TEST::getRandomU16(1,9);
        uint16_t reg2 = TEST::getRandomU16(1,9);

        while(reg1 == reg2)
        {
            reg2 = TEST::getRandomU16(1,9);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGTD)
        {
            vm->registers[reg1] = TEST::doubleToUint64(TEST::getRandomDouble(0, 200));
            vm->registers[reg2] = TEST::doubleToUint64(TEST::getRandomDouble(500, 600));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTED)
        {
            vm->registers[reg1] = TEST::doubleToUint64(9.0);
            vm->registers[reg2] = TEST::doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTD)
        {
            vm->registers[reg1] = TEST::doubleToUint64(TEST::getRandomDouble(500, 700));
            vm->registers[reg2] = TEST::doubleToUint64(TEST::getRandomDouble(100, 200));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTED)
        {
            vm->registers[reg1] = TEST::doubleToUint64(15.0);
            vm->registers[reg2] = TEST::doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BEQD)
        {
            vm->registers[reg1] = TEST::doubleToUint64(55.0);
            vm->registers[reg2] = TEST::doubleToUint64(10.0);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BNED)
        {
            vm->registers[reg1] = TEST::doubleToUint64(25.0);
            vm->registers[reg2] = TEST::doubleToUint64(25.0);
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
        TEST::build_test_vm(vm, TEST::ins_to_vec(baseIns));
        TEST::build_test_vm(vm, TEST::ins_to_vec(branchIns));

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