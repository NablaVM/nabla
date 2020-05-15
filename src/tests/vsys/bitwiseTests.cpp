#include "testSetup.hpp"

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaBitwiseTests)
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

TEST(NablaBitwiseTests, bitwiseIns)
{
    // Each of the arithmatic types (ADD ,MUL, DIV, SUB)
    for(int typesItr = 0x01; typesItr <= 0x06; typesItr++)
    {
        NABLA::Bytegen::BitwiseTypes arithType = static_cast<NABLA::Bytegen::BitwiseTypes>(typesItr);

        for(int setupItr = 0; setupItr <= 3; setupItr++)
        {
            NABLA::Bytegen::ArithmaticSetup arithSetup = static_cast<NABLA::Bytegen::ArithmaticSetup>(setupItr);

            // NOT instruction only have 2 seperate arithmatic setups, so we skip the invalids
            if(NABLA::Bytegen::BitwiseTypes::NOT == arithType)
            {
                if(NABLA::Bytegen::ArithmaticSetup::REG_NUM == arithSetup || 
                   NABLA::Bytegen::ArithmaticSetup::NUM_NUM == arithSetup)
                {
                    continue;
                }
            }

            NABLA::Bytegen bytegen;
            TEST::TestMachine vm;


            int16_t dest_reg    = TEST::getRandomU16(0, 9);
            uint64_t arg1;
            uint64_t arg2;

            uint64_t expectedResult;

            switch(arithSetup)
            {
                case NABLA::Bytegen::ArithmaticSetup::REG_REG: 
                    arg1 = TEST::getRandomU16(0, 9); vm.setReg(arg1, TEST::getRandomU16(0, 65000)); // random reg with random val
                    arg2 = TEST::getRandomU16(0, 9); vm.setReg(arg2, TEST::getRandomU16(0, 65000)); // random reg with random val

                    expectedResult = TEST::calculateBitwise(arithType, vm.getReg(arg1), vm.getReg(arg2));
                    break;

                case NABLA::Bytegen::ArithmaticSetup::REG_NUM: 
                    arg1 = TEST::getRandomU16(0, 9); vm.setReg(arg1,  TEST::getRandomU16(0, 65000)); // Random reg with random val
                    arg2 = TEST::getRandomS16(-30000, 30000);                                            // Random val

                    expectedResult = TEST::calculateBitwise(arithType, vm.getReg(arg1), arg2);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::NUM_REG: 
                    arg2 = TEST::getRandomU16(0, 9); vm.setReg(arg2, TEST::getRandomU16(0, 65000)); // Random reg with random val
                    arg1 = TEST::getRandomS16(-30000, 30000);                                            // Random val

                    expectedResult = TEST::calculateBitwise(arithType, arg1,vm.getReg(arg2));
                    break;

                case NABLA::Bytegen::ArithmaticSetup::NUM_NUM: 
                    arg1 = TEST::getRandomS16(-30000, 30000);                                             // Random val
                    arg2 = TEST::getRandomS16(-30000, 30000);                                             // Random val

                    expectedResult = TEST::calculateBitwise(arithType, arg1, arg2);
                    break;
            }

            NABLA::Bytegen::ArithmaticSetup setup;

            // Add registers r0 and r1 together, place in destination
            NABLA::Bytegen::Instruction ins = bytegen.createBitwiseInstruction(
                arithType,
                arithSetup,
                dest_reg,
                arg1,
                arg2
            );

            vm.build( TEST::ins_to_vec(ins));

            NABLA::VSYS::ExecutionReturns result = vm.step(4);
            CHECK_TRUE(NABLA::VSYS::ExecutionReturns::OKAY == result);
            CHECK_TRUE(TEST::check_result(vm, dest_reg, expectedResult));
        }
    }
}
