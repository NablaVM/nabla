#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaSizeTests)
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

TEST(NablaSizeTests, sizeInstruction)
{
    for(int i = 0; i < 100; i++)
    {
        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        NABLA::Bytegen::Stacks stackLoc = static_cast<NABLA::Bytegen::Stacks>(TEST::getRandomU16(0,1));

        // A register to get data from for push instructions used to grow the stack for testing
        uint16_t pushReg = TEST::getRandomU16(0, 9);

        // Where the size instruction should be placing size once acquired
        uint16_t dest_reg = TEST::getRandomU16(0, 9);

        // Ensure push and pop reg differ
        while(pushReg == dest_reg)
        {
            dest_reg = TEST::getRandomU16(0, 9);
        }

        // Put some random value, we really don't care what it is
        vm->registers[pushReg] = TEST::getRandomU16(0, 65530);

        // Create a size instruction that should read '0' when called 
        NABLA::Bytegen::Instruction sizeIns = bytegen.createSizeInstruction(dest_reg, stackLoc);

        std::vector<uint8_t> sizeInsBytes = TEST::ins_to_vec(sizeIns);
        TEST::build_test_vm(vm, sizeInsBytes);

        // Grow the stack some number of sizes
        uint8_t numPushs = TEST::getRandomU16(0, 55);

        for(int sp = 0; sp < numPushs; sp++)
        {
            NABLA::Bytegen::Instruction pushIns = bytegen.createPushInstruction(
                stackLoc, pushReg
            );

            std::vector<uint8_t> pushBytes = TEST::ins_to_vec(pushIns);
            TEST::build_test_vm(vm, pushBytes);
        }

        // Create a stack instruction that should read back numPushs
        sizeInsBytes.clear();
        sizeInsBytes = TEST::ins_to_vec(bytegen.createSizeInstruction(dest_reg, stackLoc));
        TEST::build_test_vm(vm, sizeInsBytes);

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
