#include "testSetup.hpp"

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
        TEST::TestMachine vm;

        vm.setReg(0, 0);

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

        std::vector<uint8_t> vins = TEST::ins_to_vec(baseIns);
        
        std::vector<uint8_t> vins1 = TEST::ins_to_vec(jmpIns);

        vins.insert(std::end(vins), std::begin(vins1), std::end(vins1));

        vm.build(vins);

        // Step 1 instruction (should be add)
        vm.step(1);

        // Sanity
        CHECK_EQUAL(1, vm.getActiveReg(0));

        // Step 2 instruction (should be jump, followed by add)
        vm.step(2);

        CHECK_EQUAL(2, vm.getActiveReg(0));
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
        TEST::TestMachine vm;

        vm.setReg(0, 0);

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );
        
        vm.setReg(9, 0);

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

        std::vector<uint8_t> vins  = TEST::ins_to_vec(jmpIns);
        std::vector<uint8_t> vins1 = TEST::ins_to_vec(baseIns);
        std::vector<uint8_t> vins2 = TEST::ins_to_vec(baseIns1);

        vins.insert(std::end(vins), std::begin(vins1), std::end(vins1));
        vins.insert(std::end(vins), std::begin(vins2), std::end(vins2));

        vm.build(vins);

        // Step 1 instruction (should be jump)
        vm.step(1);

        // Sanity
        CHECK_EQUAL(0, vm.getActiveReg(0));

        // Step 2 instruction (should be jump, followed by add to register 2)
        vm.step(2);

        // Sanity
        CHECK_EQUAL(0, vm.getActiveReg(0));  // Hopefully skipped

        CHECK_EQUAL(1, vm.getActiveReg(9));  // Hopefully added 2
    }
}