#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaBranchTests)
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

TEST(NablaBranchTests, standardBranchIns)
{
    for(int i = 0x01; i <= 0x06; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-9
        uint16_t reg1 = TEST::getRandomU16(1,9);
        uint16_t reg2 = TEST::getRandomU16(1,9);

        while(reg1 == reg2)
        {
            reg2 = TEST::getRandomU16(1,9);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGT)
        {
            vm.setReg(reg1, TEST::getRandomU16(500, 600));
            vm.setReg(reg2, TEST::getRandomU16(0  , 200));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTE)
        {
            vm.setReg(reg1, 10);
            vm.setReg(reg2, 10);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLT)
        {
            vm.setReg(reg1, TEST::getRandomU16(0  , 200));
            vm.setReg(reg2, TEST::getRandomU16(500, 600));

        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTE)
        {
            vm.setReg(reg1, 10);
            vm.setReg(reg2, 10);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BEQ)
        {
            vm.setReg(reg1, 10);
            vm.setReg(reg2, 10);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BNE)
        {
            vm.setReg(reg1, 10);
            vm.setReg(reg2, 25);
        }

        // Instruction to branch to is at location 0, so ensure 0 is there to start. Should be 1 on first cycle
            vm.setReg(0, 0);

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        NABLA::Bytegen::Instruction branchIns = bytegen.createBranchInstruction(
            type,   // Type of branch 
            reg1,   // Comparison reg 1
            reg2,   // Comparison reg 2
            0       // Location to branch to
        );

        std::vector<uint8_t> vins = TEST::ins_to_vec(baseIns);
        
        std::vector<uint8_t> vins1 = TEST::ins_to_vec(branchIns);

        vins.insert(std::end(vins), std::begin(vins1), std::end(vins1));

        vm.build(vins);

        // Step 1 instruction (should be add)
        vm.step(1);

        // Sanity
        CHECK_EQUAL(1, vm.getActiveReg(0));

        // Step 2 instruction (should be branch check, followed by add)
        vm.step(2);

        CHECK_EQUAL(2, vm.getActiveReg(0));
    }
}


// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaBranchTests, standardBranchInsExpectedFails)
{
    for(int i = 0x01; i <= 0x06; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-9
        uint16_t reg1 = TEST::getRandomU16(1,9);
        uint16_t reg2 = TEST::getRandomU16(1,9);

        while(reg1 == reg2)
        {
            reg2 = TEST::getRandomU16(1,9);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGT)
        {
            vm.setReg(reg1, TEST::getRandomU16(0  , 200));
            vm.setReg(reg2, TEST::getRandomU16(500, 600));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTE)
        {
            vm.setReg(reg1, 1);
            vm.setReg(reg2, 10);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLT)
        {
            vm.setReg(reg1, TEST::getRandomU16(500, 600));
            vm.setReg(reg2, TEST::getRandomU16(0  , 200));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTE)
        {
            vm.setReg(reg1, 55);
            vm.setReg(reg2, 10);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BEQ)
        {
            vm.setReg(reg1, 500);
            vm.setReg(reg2, 10);
        }
        else if (type == NABLA::Bytegen::BranchTypes::BNE)
        {
            vm.setReg(reg1, 25);
            vm.setReg(reg2, 25);
        }


        // Instruction to branch to is at location 0, so ensure 0 is there to start. Should be 1 on first cycle
            vm.setReg(0, 0);

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        NABLA::Bytegen::Instruction branchIns = bytegen.createBranchInstruction(
            type,   // Type of branch 
            reg1,   // Comparison reg 1
            reg2,   // Comparison reg 2
            0       // Location to branch to
        );

        std::vector<uint8_t> vins = TEST::ins_to_vec(baseIns);
        
        std::vector<uint8_t> vins1 = TEST::ins_to_vec(branchIns);

        vins.insert(std::end(vins), std::begin(vins1), std::end(vins1));

        vm.build(vins);

        // Step 1 instruction (should be add)
        vm.step(1);

        // Sanity
        CHECK_EQUAL(1, vm.getActiveReg(0));

        // Step 2 instruction (should be branch check, followed by add)
        vm.step(2);

        CHECK_EQUAL(1, vm.getActiveReg(0));
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaBranchTests, doubleBranchIns)
{
    for(int i = 0x07; i <= 0x0C; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-9
        uint16_t reg1 = TEST::getRandomU16(1,9);
        uint16_t reg2 = TEST::getRandomU16(1,9);

        while(reg1 == reg2)
        {
            reg2 = TEST::getRandomU16(1,9);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGTD)
        {
            vm.setReg(reg1, TEST::doubleToUint64(TEST::getRandomDouble(500, 600)));
            vm.setReg(reg2, TEST::doubleToUint64(TEST::getRandomDouble(0, 200)));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTED)
        {
            vm.setReg(reg1, TEST::doubleToUint64(10.0));
            vm.setReg(reg2, TEST::doubleToUint64(10.0));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTD)
        {
            vm.setReg(reg1, TEST::doubleToUint64(TEST::getRandomDouble(0, 200)));
            vm.setReg(reg2, TEST::doubleToUint64(TEST::getRandomDouble(500, 600)));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTED)
        {
            vm.setReg(reg1, TEST::doubleToUint64(10.0));
            vm.setReg(reg2, TEST::doubleToUint64(10.0));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BEQD)
        {
            vm.setReg(reg1, TEST::doubleToUint64(10.0));
            vm.setReg(reg2, TEST::doubleToUint64(10.0));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BNED)
        {
            vm.setReg(reg1, TEST::doubleToUint64(10.0));
            vm.setReg(reg2, TEST::doubleToUint64(25.0));
        }

        // Instruction to branch to is at location 0, so ensure 0 is there to start. Should be 1 on first cycle
            vm.setReg(0, 0);

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        NABLA::Bytegen::Instruction branchIns = bytegen.createBranchInstruction(
            type,   // Type of branch 
            reg1,   // Comparison reg 1
            reg2,   // Comparison reg 2
            0       // Location to branch to
        );

        std::vector<uint8_t> vins = TEST::ins_to_vec(baseIns);
        
        std::vector<uint8_t> vins1 = TEST::ins_to_vec(branchIns);

        vins.insert(std::end(vins), std::begin(vins1), std::end(vins1));

        vm.build(vins);

        // Step 1 instruction (should be add)
        vm.step(1);

        // Sanity
        CHECK_EQUAL(1, vm.getActiveReg(0));

        // Step 2 instruction (should be branch check, followed by add)
        vm.step(2);

        CHECK_EQUAL(2, vm.getActiveReg(0));
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaBranchTests, doubleBranchInsFails)
{
    for(int i = 0x07; i <= 0x0C; i++)
    {
        NABLA::Bytegen bytegen;
        TEST::TestMachine vm;

        NABLA::Bytegen::BranchTypes type = static_cast<NABLA::Bytegen::BranchTypes>(i);

        // Registers for comparisons will be 1-9
        uint16_t reg1 = TEST::getRandomU16(1,9);
        uint16_t reg2 = TEST::getRandomU16(1,9);

        while(reg1 == reg2)
        {
            reg2 = TEST::getRandomU16(1,9);
        }

        if(type == NABLA::Bytegen::BranchTypes::BGTD)
        {
            vm.setReg(reg1, TEST::doubleToUint64(TEST::getRandomDouble(0, 200)));
            vm.setReg(reg2, TEST::doubleToUint64(TEST::getRandomDouble(500, 600)));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BGTED)
        {
            vm.setReg(reg1, TEST::doubleToUint64(9.0));
            vm.setReg(reg2, TEST::doubleToUint64(10.0));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTD)
        {
            vm.setReg(reg1, TEST::doubleToUint64(TEST::getRandomDouble(500, 700)));
            vm.setReg(reg2, TEST::doubleToUint64(TEST::getRandomDouble(100, 200)));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BLTED)
        {
            vm.setReg(reg1, TEST::doubleToUint64(15.0));
            vm.setReg(reg2, TEST::doubleToUint64(10.0));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BEQD)
        {
            vm.setReg(reg1, TEST::doubleToUint64(55.0));
            vm.setReg(reg2, TEST::doubleToUint64(10.0));
        }
        else if (type == NABLA::Bytegen::BranchTypes::BNED)
        {
            vm.setReg(reg1, TEST::doubleToUint64(25.0));
            vm.setReg(reg2, TEST::doubleToUint64(25.0));
        }


        // Instruction to branch to is at location 0, so ensure 0 is there to start. Should be 1 on first cycle
            vm.setReg(0, 0);

        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        NABLA::Bytegen::Instruction branchIns = bytegen.createBranchInstruction(
            type,   // Type of branch 
            reg1,   // Comparison reg 1
            reg2,   // Comparison reg 2
            0       // Location to branch to
        );

        std::vector<uint8_t> vins = TEST::ins_to_vec(baseIns);
        
        std::vector<uint8_t> vins1 = TEST::ins_to_vec(branchIns);

        vins.insert(std::end(vins), std::begin(vins1), std::end(vins1));

        vm.build(vins);

        // Step 1 instruction (should be add)
        vm.step(1);

        // Sanity
        CHECK_EQUAL(1, vm.getActiveReg(0));

        // Step 2 instruction (should be branch check, followed by add)
        vm.step(2);

        CHECK_EQUAL(1, vm.getActiveReg(0));
    }
}