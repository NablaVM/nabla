#include "testSetup.hpp"

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
        TEST::TestMachine vm;

        vm.setReg(0, 0);

        uint16_t randomNumAdds = TEST::getRandomU16(1, 500);

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        std::vector<uint8_t> vins;

        // Add a random number of additions
        for(int i = 0; i < randomNumAdds; i++)
        {
            std::vector<uint8_t> v = TEST::ins_to_vec(baseIns);

            vins.insert(std::end(vins), std::begin(v), std::end(v));
        }

        NABLA::Bytegen::Instruction nopIns = bytegen.createNopInstruction();

        uint16_t randomNumNops = TEST::getRandomU16(1, 500);

        // Add a random number of additions
        for(int i = 0; i < randomNumNops; i++)
        {
            std::vector<uint8_t> v = TEST::ins_to_vec(nopIns);

            vins.insert(std::end(vins), std::begin(v), std::end(v));
        }

        vm.build(vins);

        // Step over the adds
        vm.step(randomNumAdds);

        // Ensure that all of the adds have occured
        CHECK_EQUAL(randomNumAdds, vm.getActiveReg(0));

        // Step the noops
        vm.step(randomNumNops);

        // Ensure the addition sum is the same as before noops
        CHECK_EQUAL(randomNumAdds, vm.getActiveReg(0));
    }
}
