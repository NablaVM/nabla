#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaMovTests)
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

TEST(NablaMovTests, movIns)
{
    for(int i = 0; i < 10; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

        NABLA::Bytegen::MovSetup setup = static_cast<NABLA::Bytegen::MovSetup>(TEST::getRandomU16(0,1));

        int16_t dest_reg    = TEST::getRandomU16(0, 9);

        uint64_t arg1;
        uint64_t expectedResult;

        if(setup == NABLA::Bytegen::MovSetup::REG_REG)
        {
            arg1 = TEST::getRandomU16(0, 9); 
            expectedResult = (int8_t)TEST::getRandomS16(-120, 120);

            vm.setReg(arg1, expectedResult);
        }
        else
        {
            // REG_NUM needs to ensure int8_t as the numerical encoding is limited to int8_t
            expectedResult =  (int8_t)TEST::getRandomS16(-120, 120);
            arg1 = expectedResult;
        } 

        NABLA::Bytegen::Instruction ins = bytegen.createMovInstruction(
            setup,
            dest_reg,
            arg1
        );

        std::vector<uint8_t> vins = TEST::ins_to_vec(ins);
        
        vm.build(vins);

        vm.step(1);

        CHECK_TRUE(TEST::check_result(vm, dest_reg, expectedResult));
    }
}
