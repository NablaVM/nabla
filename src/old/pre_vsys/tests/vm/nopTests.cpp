#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaNopTests)
{
    void setup()
    {
     //   MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    }

    void teardown()
    {
       // MemoryLeakWarningPlugin::turnOnNewDeleteOverloads();
    }
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaNopTests, nopIns)
{
    for(int t = 0; t < 100; t++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        vm->registers[0]    = 0;

        uint16_t randomNumAdds = TEST::getRandomU16(1, 500);

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
            TEST::build_test_vm(vm, TEST::ins_to_vec(baseIns));
        }

        NABLA::Bytegen::Instruction nopIns = bytegen.createNopInstruction();

        uint16_t randomNumNops = TEST::getRandomU16(1, 500);

        // Add a random number of additions
        for(int i = 0; i < randomNumNops; i++)
        {
            TEST::build_test_vm(vm, TEST::ins_to_vec(nopIns));
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
