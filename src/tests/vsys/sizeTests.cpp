#include "testSetup.hpp"

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

TEST(NablaSizeTests, sizeSingleByteInstruction)
{
    for(int i = 0; i < 100; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

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
        vm.setReg(pushReg, TEST::getRandomU16(0, 65530));

        // Create a size instruction that should read '0' when called 
        NABLA::Bytegen::Instruction sizeIns = bytegen.createSizeInstruction(dest_reg, stackLoc);

        std::vector<uint8_t> vins = TEST::ins_to_vec(sizeIns);

        // Grow the stack some number of sizes
        uint8_t numPushs = TEST::getRandomU16(0, 55);

        for(int sp = 0; sp < numPushs; sp++)
        {
            NABLA::Bytegen::Instruction pushIns = bytegen.createPushInstruction(
                stackLoc, pushReg
            );

            std::vector<uint8_t> pushBytes = TEST::ins_to_vec(pushIns);

            vins.insert(std::end(vins), std::begin(pushBytes), std::end(pushBytes));
        }

        std::vector<uint8_t> vins1 = TEST::ins_to_vec(bytegen.createSizeInstruction(dest_reg, stackLoc));
        
        vins.insert(std::end(vins), std::begin(vins1), std::end(vins1));

        vm.build(vins);


        // Step 1 instruction (should be push)
        vm.step(1);

        // Size should have read 0
        CHECK_TRUE(TEST::check_result(vm, dest_reg, 0));

        // Step all of the push instructions
        vm.step(numPushs);
     
        // Sanity check
        CHECK_TRUE(TEST::check_result(vm, dest_reg, 0));
        
        // Execute the 2nd size instruction
        vm.step(1);

        // Make sure its reading a size of 'numPushs'
        CHECK_TRUE(TEST::check_result(vm, dest_reg, numPushs));
    }

}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaSizeTests, sizeMultipleByteInstruction)
{
    for(int i = 0; i < 100; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

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
        vm.setReg(pushReg, TEST::getRandomU16(0, 65530));

        // Create a size instruction that should read '0' when called 
        NABLA::Bytegen::Instruction sizeIns = bytegen.createSizeInstruction(dest_reg, stackLoc);

        std::vector<uint8_t> vins = TEST::ins_to_vec(sizeIns);

        // Grow the stack some number of sizes
        uint8_t numPushs = TEST::getRandomU16(0, 55);

        for(int sp = 0; sp < numPushs; sp++)
        {
            NABLA::Bytegen::Instruction pushIns = bytegen.createPushwInstruction(
                stackLoc, pushReg
            );

            std::vector<uint8_t> pushBytes = TEST::ins_to_vec(pushIns);

            vins.insert(std::end(vins), std::begin(pushBytes), std::end(pushBytes));
        }

        std::vector<uint8_t> vins1 = TEST::ins_to_vec(bytegen.createSizeInstruction(dest_reg, stackLoc));
        
        vins.insert(std::end(vins), std::begin(vins1), std::end(vins1));

        vm.build(vins);


        // Step 1 instruction (should be push)
        vm.step(1);

        // Size should have read 0
        CHECK_TRUE(TEST::check_result(vm, dest_reg, 0));

        // Step all of the push instructions
        vm.step(numPushs);
     
        // Sanity check
        CHECK_TRUE(TEST::check_result(vm, dest_reg, 0));
        
        // Execute the 2nd size instruction
        vm.step(1);

        // Make sure its reading a size of 'numPushs' * 8 (because we pushw'd)
        CHECK_TRUE(TEST::check_result(vm, dest_reg, numPushs * 8));
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaSizeTests, sizeMixedByteInstruction)
{
    for(int i = 0; i < 100; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

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
        vm.setReg(pushReg, TEST::getRandomU16(0, 65530));

        // Create a size instruction that should read '0' when called 
        NABLA::Bytegen::Instruction sizeIns = bytegen.createSizeInstruction(dest_reg, stackLoc);

        std::vector<uint8_t> vins = TEST::ins_to_vec(sizeIns);

        // Grow the stack some number of sizes
        uint8_t numPushs = TEST::getRandomU16(0, 55);

        uint64_t expectedResult = 0;

        for(int sp = 0; sp < numPushs; sp++)
        {
            if(TEST::getRandomU16(0,1))
            {
                // Push w
                NABLA::Bytegen::Instruction pushIns = bytegen.createPushwInstruction(
                    stackLoc, pushReg
                );

                std::vector<uint8_t> pushBytes = TEST::ins_to_vec(pushIns);

                vins.insert(std::end(vins), std::begin(pushBytes), std::end(pushBytes));

                expectedResult += 8;
            }
            else
            {
                // Regular push
                NABLA::Bytegen::Instruction pushIns = bytegen.createPushInstruction(
                    stackLoc, pushReg
                );

                std::vector<uint8_t> pushBytes = TEST::ins_to_vec(pushIns);

                vins.insert(std::end(vins), std::begin(pushBytes), std::end(pushBytes));
                
                expectedResult += 1;
            }
        }

        std::vector<uint8_t> vins1 = TEST::ins_to_vec(bytegen.createSizeInstruction(dest_reg, stackLoc));
        
        vins.insert(std::end(vins), std::begin(vins1), std::end(vins1));

        vm.build(vins);

        // Step 1 instruction (should be push)
        vm.step(1);

        // Size should have read 0
        CHECK_TRUE(TEST::check_result(vm, dest_reg, 0));

        // Step all of the push instructions
        vm.step(numPushs);
     
        // Sanity check
        CHECK_TRUE(TEST::check_result(vm, dest_reg, 0));
        
        // Execute the 2nd size instruction
        vm.step(1);

        // Make sure its reading a size of 'numPushs' * 8 (because we pushw'd)
        CHECK_TRUE(TEST::check_result(vm, dest_reg, expectedResult));
    }
}

// ---------------------------------------------------------------
//  Push and pop mixed
// ---------------------------------------------------------------

TEST(NablaSizeTests, pushPopMixedByteInstruction)
{
    for(int i = 0; i < 100; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

        NABLA::Bytegen::Stacks stackLoc = static_cast<NABLA::Bytegen::Stacks>(TEST::getRandomU16(0,1));

        // A register to get data from for push instructions used to grow the stack for testing
        uint16_t pushReg = TEST::getRandomU16(0, 8);

        // Where the size instruction should be placing size once acquired
        uint16_t dest_reg = TEST::getRandomU16(0, 8);

        // Ensure push and pop reg differ
        while(pushReg == dest_reg)
        {
            dest_reg = TEST::getRandomU16(0, 8);
        }

        // Put some random value, we really don't care what it is
        vm.setReg(pushReg, TEST::getRandomU16(0, 65530));

        // Create a size instruction that should read '0' when called 
        NABLA::Bytegen::Instruction sizeIns = bytegen.createSizeInstruction(dest_reg, stackLoc);

        std::vector<uint8_t> vins = TEST::ins_to_vec(sizeIns);

        // Grow the stack some number of sizes
        uint8_t numPushs = TEST::getRandomU16(0, 55);

        int expectedResult = 0;

        uint64_t numInstructs = 0;

        for(int sp = 0; sp < numPushs; sp++)
        {
            if(TEST::getRandomU16(0,1))
            {
                numInstructs++;

                // Push w
                NABLA::Bytegen::Instruction pushIns = bytegen.createPushwInstruction(
                    stackLoc, pushReg
                );

                std::vector<uint8_t> pushBytes = TEST::ins_to_vec(pushIns);

                vins.insert(std::end(vins), std::begin(pushBytes), std::end(pushBytes));

                expectedResult += 8;
            }
            else
            {
                numInstructs++;

                // Regular push
                NABLA::Bytegen::Instruction pushIns = bytegen.createPushInstruction(
                    stackLoc, pushReg
                );

                std::vector<uint8_t> pushBytes = TEST::ins_to_vec(pushIns);

                vins.insert(std::end(vins), std::begin(pushBytes), std::end(pushBytes));
                
                expectedResult += 1;
            }

            if(TEST::getRandomU16(0,1))
            {
                if(expectedResult - 8 >= 0)
                {
                    numInstructs++;

                    // Pop w
                    NABLA::Bytegen::Instruction popIns = bytegen.createPopwInstruction(
                        stackLoc, 9
                    );

                    std::vector<uint8_t> popBytes = TEST::ins_to_vec(popIns);

                    vins.insert(std::end(vins), std::begin(popBytes), std::end(popBytes));

                    expectedResult -= 8;
                }
            }
            else
            {
                if(expectedResult - 1 >= 0 )
                {
                    numInstructs++;

                    // Pop 
                    NABLA::Bytegen::Instruction popIns = bytegen.createPopInstruction(
                        stackLoc, 9
                    );

                    std::vector<uint8_t> popBytes = TEST::ins_to_vec(popIns);

                    vins.insert(std::end(vins), std::begin(popBytes), std::end(popBytes));

                    expectedResult -= 1;
                }
            }
        }

        std::vector<uint8_t> vins1 = TEST::ins_to_vec(bytegen.createSizeInstruction(dest_reg, stackLoc));
        
        vins.insert(std::end(vins), std::begin(vins1), std::end(vins1));

        vm.build(vins);

        // Step 1 instruction (should be push)
        vm.step(1);

        // Size should have read 0
        CHECK_TRUE(TEST::check_result(vm, dest_reg, 0));

        // Step all of the push instructions
        vm.step(numInstructs);
     
        // Sanity check
        CHECK_TRUE(TEST::check_result(vm, dest_reg, 0));
        
        // Execute the 2nd size instruction
        vm.step(1);

        // Make sure its reading a size of 'numPushs' * 8 (because we pushw'd)
        CHECK_TRUE(TEST::check_result(vm, dest_reg, expectedResult));
    }
}