#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaJumpTests)
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

TEST(NablaJumpTests, jumpInsLow)
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

        TEST::build_test_vm(vm, TEST::ins_to_vec(baseIns));
        TEST::build_test_vm(vm, TEST::ins_to_vec(jmpIns));

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

TEST(NablaJumpTests, jumpInsHigh)
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

        TEST::build_test_vm(vm, TEST::ins_to_vec(jmpIns));
        TEST::build_test_vm(vm, TEST::ins_to_vec(baseIns)); // This one is jumped over
        TEST::build_test_vm(vm, TEST::ins_to_vec(baseIns1));

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