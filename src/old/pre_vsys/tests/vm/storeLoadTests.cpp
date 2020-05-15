#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaStoreLoadTests)
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

TEST(NablaStoreLoadTests, stbLdbIns)
{
    for(int i = 0; i < 10; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::Stacks stackLoc = static_cast<NABLA::Bytegen::Stacks>(TEST::getRandomU16(0,1));

        uint16_t reg = TEST::getRandomU16(0, 8);

        vm->registers[reg] = TEST::getRandomU16(0, 65530);

        NABLA::Bytegen::Instruction storeIns;
        NABLA::Bytegen::Instruction loadIns;

        // Randomly decide if we should use register based, or number based store / load instructions
        if(TEST::getRandomU16(0, 1) == 0)
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

            uint16_t sourceReg = TEST::getRandomU16(0, 8);

            while(reg == sourceReg)
            {
                sourceReg = TEST::getRandomU16(0, 8);
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
        std::vector<uint8_t> storeBytes = TEST::ins_to_vec(storeIns);
        std::vector<uint8_t> loadBytes  = TEST::ins_to_vec(loadIns);

        // Populate vm
        TEST::build_test_vm(vm, storeBytes);
        TEST::build_test_vm(vm, loadBytes);

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
