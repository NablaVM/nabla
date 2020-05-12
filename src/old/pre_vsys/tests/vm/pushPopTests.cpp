#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaPushPopTests)
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

TEST(NablaPushPopTests, pushPopIns)
{
    for(int i = 0; i < 10; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::Stacks stackLoc = static_cast<NABLA::Bytegen::Stacks>(TEST::getRandomU16(0,1));

        uint16_t pushReg = TEST::getRandomU16(0, 9);
        uint16_t popReg  = TEST::getRandomU16(0, 9);

        // Ensure push and pop reg differ
        while(pushReg == popReg)
        {
            popReg = TEST::getRandomU16(0, 9);
        }

        vm->registers[pushReg] = TEST::getRandomU16(0, 65530);

        NABLA::Bytegen::Instruction pushIns = bytegen.createPushInstruction(
            stackLoc, pushReg
        );

        NABLA::Bytegen::Instruction popIns = bytegen.createPopInstruction(
            stackLoc, popReg
        );

        std::vector<uint8_t> pushBytes = TEST::ins_to_vec(pushIns);
        std::vector<uint8_t> popBytes  = TEST::ins_to_vec(popIns);

        // Populate vm
        TEST::build_test_vm(vm, pushBytes);
        TEST::build_test_vm(vm, popBytes);

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
