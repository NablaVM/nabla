#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaYieldTests)
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

TEST(NablaYieldTests, yieldTests)
{
    /*
        To test yield, we create two functions :

        <func_zero:
            call func_one

            mov r9 (random value)

            call func_one
        >
    
        <func_one:
            add r0 r0 $1

            yield

            add r0 r0 $1
        >
    
    */

    NABLA::Bytegen bytegen;
    
    NablaVirtualMachine vm = vm_new();

    vm->fp = 0;
    vm->entryAddress = 0;
    vm->numberOfFunctions = 2;

    /*
    
            Create call to function 1
    
    */

    // create a call to function 1 that will yield after one instruction
    {
        std::vector<NABLA::Bytegen::Instruction> callIns = bytegen.createCallInstruction(
            0, // Function from  
            3, // Return-to instruction
            1  // Function to call
        );

        for(auto &ei : callIns)
        {
            std::vector<uint8_t> callBytes = TEST::ins_to_vec(ei);

            for(uint64_t ins = 0; ins < callBytes.size()/8; ins++)
            {
                uint64_t currentIns = 0;
                for(int n = 7; n >= 0; n--)   
                {
                    currentIns |= (uint64_t)callBytes[ins++] << (n * 8);
                }

                int pushResult;
                stack_push(currentIns, vm->functions[0].instructions, &pushResult);

                if(pushResult != STACK_OKAY)
                {
                    FAIL("Failed to push instruction to vm");
                }
            }
        }
    }

    int8_t expectedMoveResult = (int8_t)TEST::getRandomS16(-120, 120);
    
    // create a mov instruction to tell us if we're back in function 0
    {
        
        NABLA::Bytegen::Instruction ins = bytegen.createMovInstruction(
            NABLA::Bytegen::MovSetup::REG_NUM,
            9,                  // Destination register
            expectedMoveResult  // The value to move into the register
        );

        std::vector<uint8_t> instructions = TEST::ins_to_vec(ins);

        for(uint64_t ins = 0; ins < instructions.size()/8; ins++)
        {
            uint64_t currentIns = 0;
            for(int n = 7; n >= 0; n--)   
            {
                currentIns |= (uint64_t)instructions[ins++] << (n * 8);
            }

            int pushResult;
            stack_push(currentIns, vm->functions[0].instructions, &pushResult);

            if(pushResult != STACK_OKAY)
            {
                FAIL("Failed to push instruction to vm");
            }
        }
    }

    // create a call to function 1 that will return
    {
        std::vector<NABLA::Bytegen::Instruction> callIns = bytegen.createCallInstruction(
            0, // Function from  
            7, // Return-to instruction
            1  // Function to call
        );

        for(auto &ei : callIns)
        {
            std::vector<uint8_t> callBytes = TEST::ins_to_vec(ei);

            for(uint64_t ins = 0; ins < callBytes.size()/8; ins++)
            {
                uint64_t currentIns = 0;
                for(int n = 7; n >= 0; n--)   
                {
                    currentIns |= (uint64_t)callBytes[ins++] << (n * 8);
                }

                int pushResult;
                stack_push(currentIns, vm->functions[0].instructions, &pushResult);

                if(pushResult != STACK_OKAY)
                {
                    FAIL("Failed to push instruction to vm");
                }
            }
        }
    }


    /*
    
            Create function 1
    
    */

    // Setup an add instruction
    {
        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        std::vector<uint8_t> instructions = TEST::ins_to_vec(baseIns);

        for(uint64_t ins = 0; ins < instructions.size()/8; ins++)
        {
            uint64_t currentIns = 0;
            for(int n = 7; n >= 0; n--)   
            {
                currentIns |= (uint64_t)instructions[ins++] << (n * 8);
            }

            int pushResult;
            stack_push(currentIns, vm->functions[1].instructions, &pushResult);

            if(pushResult != STACK_OKAY)
            {
                FAIL("Failed to push instruction to vm");
            }

        }
    }

    // create a yield instruction
    {
        NABLA::Bytegen::Instruction baseIns = bytegen.createYieldInstruction();

        std::vector<uint8_t> instructions = TEST::ins_to_vec(baseIns);

        for(uint64_t ins = 0; ins < instructions.size()/8; ins++)
        {
            uint64_t currentIns = 0;
            for(int n = 7; n >= 0; n--)   
            {
                currentIns |= (uint64_t)instructions[ins++] << (n * 8);
            }

            int pushResult;
            stack_push(currentIns, vm->functions[1].instructions, &pushResult);

            if(pushResult != STACK_OKAY)
            {
                FAIL("Failed to push instruction to vm");
            }

        }
    }

    // Setup another add instruction
    {
        NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
            NABLA::Bytegen::ArithmaticTypes::ADD,
            NABLA::Bytegen::ArithmaticSetup::REG_NUM,
            0,  // Destination register
            0,  // reg 0
            1   // Inc reg1 by 1 every time this instruction is called
        );

        std::vector<uint8_t> instructions = TEST::ins_to_vec(baseIns);

        for(uint64_t ins = 0; ins < instructions.size()/8; ins++)
        {
            uint64_t currentIns = 0;
            for(int n = 7; n >= 0; n--)   
            {
                currentIns |= (uint64_t)instructions[ins++] << (n * 8);
            }

            int pushResult;
            stack_push(currentIns, vm->functions[1].instructions, &pushResult);

            if(pushResult != STACK_OKAY)
            {
                FAIL("Failed to push instruction to vm");
            }

        }
    }
    // --------------------------------------------------------------------

    // Init
    vm_init(vm);

    // Step 3 - Step call and the spooky hidden instructions it makes
    vm_step(vm, 4);

    // Ensure we are in the called function
    CHECK_EQUAL(1, vm->fp);

    // Check for function 1 add 
    CHECK_EQUAL(1, vm->registers[0]);

    // Yeild instruction
    vm_step(vm, 1);

    // Ensure we are in the original function
    CHECK_EQUAL(0, vm->fp);

    // Move instruction
    vm_step(vm, 1);

    // Check move result
    CHECK_EQUAL(expectedMoveResult, vm->registers[9]);
    
    // Step 3 to get into func 1
    vm_step(vm, 3);
    
    CHECK_EQUAL(1, vm->fp);

    // Trigger add instruction
    vm_step(vm, 1);

    // Check add 
    CHECK_EQUAL(2, vm->registers[0]);

    // Check for return
    vm_step(vm, 1);
    CHECK_EQUAL(0, vm->fp);

    // If the previous test passes. Then we are complete.
    vm_delete(vm);
}