#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaArithTests)
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

TEST(NablaArithTests, standardArith)
{
    // Each of the arithmatic types (ADD ,MUL, DIV, SUB)
    for(int typesItr = 0x00; typesItr <= 0x03; typesItr++)
    {
        NABLA::Bytegen::ArithmaticTypes arithType = static_cast<NABLA::Bytegen::ArithmaticTypes>(typesItr);

        for(int setupItr = 0; setupItr <= 3; setupItr++)
        {
            NABLA::Bytegen bytegen;
            NablaVirtualMachine vm = vm_new();

            NABLA::Bytegen::ArithmaticSetup arithSetup = static_cast<NABLA::Bytegen::ArithmaticSetup>(setupItr);

            int16_t dest_reg    = TEST::getRandomU16(0, 9);
            uint64_t arg1;
            uint64_t arg2;

            uint64_t expectedResult;

            switch(arithSetup)
            {
                case NABLA::Bytegen::ArithmaticSetup::REG_REG: 
                    arg1 = TEST::getRandomU16(0, 9); vm->registers[arg1] = TEST::getRandomU16(0, 65000); // random reg with random val
                    arg2 = TEST::getRandomU16(0, 9); vm->registers[arg2] = TEST::getRandomU16(0, 65000); // random reg with random val

                    expectedResult = TEST::calculateArith(arithType, vm->registers[arg1], vm->registers[arg2]);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::REG_NUM: 
                    arg1 = TEST::getRandomU16(0, 9); vm->registers[arg1] = TEST::getRandomU16(0, 65000); // Random reg with random val
                    arg2 = TEST::getRandomS16(-30000, 30000);                                            // Random val

                    expectedResult = TEST::calculateArith(arithType, vm->registers[arg1], arg2);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::NUM_REG: 
                    arg2 = TEST::getRandomU16(0, 9); vm->registers[arg2] = TEST::getRandomU16(0, 65000); // Random reg with random val
                    arg1 = TEST::getRandomS16(-30000, 30000);                                       // Random val

                    expectedResult = TEST::calculateArith(arithType, arg1, vm->registers[arg2]);
                    break;

                case NABLA::Bytegen::ArithmaticSetup::NUM_NUM: 
                    arg1 = TEST::getRandomS16(-30000, 30000);                                            // Random val
                    arg2 = TEST::getRandomS16(-30000, 30000);                                            // Random val

                    expectedResult = TEST::calculateArith(arithType, arg1, arg2);
                    break;
            }

            NABLA::Bytegen::ArithmaticSetup setup;

            // Add registers r0 and r1 together, place in destination
            NABLA::Bytegen::Instruction ins = bytegen.createArithmaticInstruction(
                arithType,
                arithSetup,
                dest_reg,
                arg1,
                arg2
            );

            TEST::build_test_vm(vm, TEST::ins_to_vec(ins));

            vm_run(vm);

            CHECK_TRUE(TEST::check_result(vm, dest_reg, expectedResult));

            vm_delete(vm);
        }
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaArithTests, doubleArith)
{
    // Each of the arithmatic types (ADD ,MUL, DIV, SUB)
    for(int typesItr = 0x04; typesItr <= 0x07; typesItr++)
    {
        NABLA::Bytegen::ArithmaticTypes arithType = static_cast<NABLA::Bytegen::ArithmaticTypes>(typesItr);

        NABLA::Bytegen bytegen;
        NablaVirtualMachine vm = vm_new();

        // The only valid double arith
        NABLA::Bytegen::ArithmaticSetup arithSetup = NABLA::Bytegen::ArithmaticSetup::REG_REG;

        int16_t dest_reg    = TEST::getRandomU16(0, 9);
        uint64_t arg1;
        uint64_t arg2;

        uint64_t expectedResult;

        switch(arithSetup)
        {
            case NABLA::Bytegen::ArithmaticSetup::REG_REG: 
                arg1 = TEST::getRandomU16(0, 9); vm->registers[arg1] = TEST::doubleToUint64(TEST::getRandomDouble(0.0, 65000.0)); // random reg with random val
                arg2 = TEST::getRandomU16(0, 9); vm->registers[arg2] = TEST::doubleToUint64(TEST::getRandomDouble(0.0, 65000.0)); // random reg with random val

                expectedResult = TEST::calculateArith(arithType, vm->registers[arg1], vm->registers[arg2]);
                break;

            case NABLA::Bytegen::ArithmaticSetup::REG_NUM: 
                arg1 = TEST::getRandomU16(0, 9); vm->registers[arg1] =TEST::doubleToUint64(TEST::getRandomDouble(0.0, 65000.0)); // Random reg with random val
                arg2 = TEST::doubleToUint64(TEST::getRandomDouble(0.0, 65000.0));                                          // Random val

                expectedResult = TEST::calculateArith(arithType, vm->registers[arg1], arg2);
                break;

            case NABLA::Bytegen::ArithmaticSetup::NUM_REG: 
                arg2 = TEST::getRandomU16(0, 9); vm->registers[arg2] = TEST::doubleToUint64(TEST::getRandomDouble(0.0, 65000.0)); // Random reg with random val
                arg1 = TEST::doubleToUint64(TEST::getRandomDouble(0.0, 65000.0));                                           // Random val

                expectedResult = TEST::calculateArith(arithType, arg1, vm->registers[arg2]);
                break;

            case NABLA::Bytegen::ArithmaticSetup::NUM_NUM: 
                arg1 = TEST::doubleToUint64(TEST::getRandomDouble(0.0, 65000.0));                                            // Random val
                arg2 = TEST::doubleToUint64(TEST::getRandomDouble(0.0, 65000.0));                                            // Random val

                expectedResult = TEST::calculateArith(arithType, arg1, arg2);
                break;
        }

        NABLA::Bytegen::ArithmaticSetup setup;

        // Add registers r0 and r1 together, place in destination
        NABLA::Bytegen::Instruction ins = bytegen.createArithmaticInstruction(
            arithType,
            arithSetup,
            dest_reg,
            arg1,
            arg2
        );

        TEST::build_test_vm(vm, TEST::ins_to_vec(ins));

        vm_run(vm);

        CHECK_TRUE(TEST::check_double_equal(TEST::uint64ToDouble(vm->registers[dest_reg]), TEST::uint64ToDouble(expectedResult)));

        vm_delete(vm);
    }
}