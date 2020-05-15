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

TEST(NablaPushPopTests, pushwPopwIns)
{
    for(int i = 0; i < 10; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

        NABLA::Bytegen::Stacks stackLoc = NABLA::Bytegen::Stacks::GLOBAL;

        uint16_t pushReg = TEST::getRandomU16(0, 9);
        uint16_t popReg  = TEST::getRandomU16(0, 9);

        // Ensure push and pop reg differ
        while(pushReg == popReg)
        {
            popReg = TEST::getRandomU16(0, 9);
        }

        vm.setReg(pushReg, TEST::getRandomU16(0, 65530));

        NABLA::Bytegen::Instruction pushIns = bytegen.createPushwInstruction(
            stackLoc, pushReg
        );

        NABLA::Bytegen::Instruction popIns = bytegen.createPopwInstruction(
            stackLoc, popReg
        );

        std::vector<uint8_t> pushBytes = TEST::ins_to_vec(pushIns);
        std::vector<uint8_t> popBytes  = TEST::ins_to_vec(popIns);

        pushBytes.insert(std::end(pushBytes), std::begin(popBytes), std::end(popBytes));

        // Init
        vm.build(pushBytes);

        // Step 1 instruction (should be push)
        vm.step(1);

        // Depending on the stack, get the value that should have been pushed
        int64_t val;
        int result = 0;
        if(stackLoc == NABLA::Bytegen::Stacks::GLOBAL)
        {
            val = vm.getGlobalWord(0);
        }
        else
        {
            // Local memory no longer reachable from execution context.
            assert(false);
        }

        CHECK_EQUAL((int64_t)vm.getActiveReg(pushReg), val);

        // Step again to execute pop
        vm.step(1);

        // See if correct val is stored
        CHECK_EQUAL(val, vm.getActiveReg(popReg));
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaPushPopTests, pushPopIns)
{
    for(int i = 0; i < 10; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

        NABLA::Bytegen::Stacks stackLoc = NABLA::Bytegen::Stacks::GLOBAL;

        uint16_t pushReg = TEST::getRandomU16(0, 9);
        uint16_t popReg  = TEST::getRandomU16(0, 9);

        // Ensure push and pop reg differ
        while(pushReg == popReg)
        {
            popReg = TEST::getRandomU16(0, 9);
        }

        uint8_t expectVal =TEST::getRandomU16(0, 250) & 0xFF;
        vm.setReg(pushReg, expectVal);

        NABLA::Bytegen::Instruction pushIns = bytegen.createPushInstruction(
            stackLoc, pushReg
        );

        NABLA::Bytegen::Instruction popIns = bytegen.createPopInstruction(
            stackLoc, popReg
        );

        std::vector<uint8_t> pushBytes = TEST::ins_to_vec(pushIns);
        std::vector<uint8_t> popBytes  = TEST::ins_to_vec(popIns);

        pushBytes.insert(std::end(pushBytes), std::begin(popBytes), std::end(popBytes));

        // Init
        vm.build(pushBytes);

        // Step 1 instruction (should be push)
        vm.step(1);

        // Depending on the stack, get the value that should have been pushed
        uint8_t val;
        int result = 0;
        if(stackLoc == NABLA::Bytegen::Stacks::GLOBAL)
        {
            val = vm.getGlobalByte(0);
        }
        else
        {
            assert(false);
            // Local memory no longer reachable from execution context.
        }

        CHECK_EQUAL((int64_t)(vm.getActiveReg(pushReg) ), (int64_t)val);

        // Step again to execute pop
        vm.step(1);

        // See if correct val is stored
        CHECK_EQUAL(val, vm.getActiveReg(popReg));
    }
}
