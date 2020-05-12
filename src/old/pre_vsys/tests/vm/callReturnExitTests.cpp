#include "testSetup.hpp"

namespace
{
    typedef struct VM * NablaVirtualMachine;
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaCallReturnExit)
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

TEST(NablaCallReturnExit, callReturnIns)
{
    NABLA::Bytegen bytegen;
    
    NablaVirtualMachine vm = vm_new();

    vm->fp = 0;
    vm->entryAddress = 0;
    vm->numberOfFunctions = 2;

    std::vector<NABLA::Bytegen::Instruction> callIns = bytegen.createCallInstruction(
        0, // Function from  
        1, // Return-to instruction
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

    // --------------------------------------------------------------------

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

    // --------------------------------------------------------------------

    // Init
    vm_init(vm);

    // Step again to execute load
    vm_step(vm, 3);

    // Ensure we have entered new 'called' function
    CHECK_EQUAL(1, vm->fp)

    vm_step(vm, 3);
    
    // See if correct val is loaded by the function that was called
    CHECK_EQUAL(1, vm->registers[0]);

    // Ensure fp came back to 0
    CHECK_EQUAL(0, vm->fp)

    vm_delete(vm);
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaCallReturnExit, exitIns)
{
    NABLA::Bytegen bytegen;
    NablaVirtualMachine vm = vm_new();

    vm->registers[0]    = 0;

    NABLA::Bytegen::Instruction baseIns = bytegen.createArithmaticInstruction(
        NABLA::Bytegen::ArithmaticTypes::ADD,
        NABLA::Bytegen::ArithmaticSetup::REG_NUM,
        0,  // Destination register
        0,  // reg 0
        1   // Inc reg1 by 1 every time this instruction is called
    );

    NABLA::Bytegen::Instruction exitIns = bytegen.createExitInstruction();

    NABLA::Bytegen::Instruction baseIns1 = bytegen.createArithmaticInstruction(
        NABLA::Bytegen::ArithmaticTypes::ADD,
        NABLA::Bytegen::ArithmaticSetup::REG_NUM,
        0,  // Destination register
        0,  // reg 0
        1   // Inc reg1 by 1 every time this instruction is called
    );

    TEST::build_test_vm(vm, TEST::ins_to_vec(baseIns));
    TEST::build_test_vm(vm, TEST::ins_to_vec(exitIns));
    TEST::build_test_vm(vm, TEST::ins_to_vec(baseIns1));

    // Init
    vm_init(vm);

    // Step 1 instruction (should be add)
    vm_step(vm, 1);

    // Sanity
    CHECK_EQUAL(1, vm->registers[0]);

    // Step a bunch and see if we ever trigger the 
    // next add instruction (shouldnt be reachable due to exit)
    vm_step(vm, 10);

    CHECK_EQUAL(1, vm->registers[0]);

    // If the previous test passes. Then we are complete.
    vm_delete(vm);
}