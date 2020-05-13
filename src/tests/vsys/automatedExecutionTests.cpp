/*
    This test is pretty wild. The core concept is that we take ASM files in strings
    and write them to file. Then we assemble them into a binary, and then load a vm.
    In this test there are 'execution steps' or 'byte code?' that drives a swtich statement
    that automates the VM tests the same way that the VM executes byte code. 

    This test ensures that the loadable machine can be loaded, stepped, etc. It also
    functions as a test of yield, call, store, and loads.

    In the future I'd like to add more tests here just to increase coverage
*/

#include "VSysLoadableMachine.hpp"
#include "VSysExecutionContext.hpp"

#include "bytegen.hpp"
#include "assembler.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <fstream>

#include "testSetup.hpp"    // For conversion methods

#include "CppUTest/TestHarness.h"

#ifdef TARGET_PLATFORM_IS_WINDOWS
    const std::string asm_file = "tmp_nabla_loadable_test_file.asm";
    const std::string bin_file = "tmp_nabla_loadable_test_binary.out";
#else 
    const std::string asm_file = "/tmp/nabla_loadable_test_file.asm";
    const std::string bin_file = "/tmp/nabla_loadable_test_binary.out";
#endif

namespace
{
    //  Instructions for testing 
    //
    enum class TestInstructs
    {
        STEP,
        CHECK_REG
    };

    struct TestInstructPairs
    {
        TestInstructs instruct;
        int data;
        int64_t expected;
    };

    const std::string ASM_YIELD =
            ".file \"yield\"\n"
            ".init main\n"
            "<main:\n"
            "    call f \n"
            "    mov r0 $99\n"
            "    call f\n"
            "    call f\n"
            ">\n"
            "<f:\n"
            "    mov r0 $1\n"
            "    yield\n"
            "    mov r0 $2\n"
            ">\n";

    const std::string ASM_LOAD_STORE =
            ".file \"newoffsets\"\n"
            ".init main\n"
            ".int8  small 42\n"
            ".int8  tiny  9\n"
            ".int64 nice  69\n"
            "<main:\n"
            "  ldb r0 $0(gs)\n"
            "  ldw r1 $2(gs)\n"
            "  stb $0(gs) r0\n"
            "  stw $2(gs) r1\n"
            "  ldb r0 $0(gs)\n"
            "  ldw r1 $2(gs)\n"
            ">\n";

    const std::string ASM_STD_ARITH = 
            ".file \"arith\"\n"
            ".init main\n"
            "<main:\n"
            "    add r0 $2 $10    ; 12 \n"
            "    add r0 r0  r0    ; 24\n"
            "    add r1 r0  $6    ; 30\n"
            "    add r0 r1  $10   ; 40\n"
            "    sub r0 $10 $2    ; 8\n"
            "    sub r0 r0  r0    ; 0\n"
            "    sub r1 r0  $6    ; -6\n"
            "    sub r0 $10 r1    ; 16\n"
            "    mul r0 $2 $2     ; 4\n"
            "    mul r0 r0 r0     ; 16\n"
            "    mul r1 r0 $2     ; 32\n"
            "    mul r1 $4 r1     ; 128\n"
            "    div r0 $100 $5   ; 20\n"
            "    div r0 r0 r0     ; 1\n"
            "    add r0 r0 $49    ; 50\n"
            "    div r1 r0 $2     ; 25\n"
            "    div r1 $100 r1   ; 4\n"
            ">\n";

    const std::string ASM_DBL_ARITH =
            ".file  \"dbl\"\n"
            ".init  main\n"
            ".int64 integer     96\n"
            ".int64 integer1    42\n"
            ".int64 integer2    55\n"
            ".int64 integer3    568888\n"
            ".double someDouble 45.435\n"
            ".double lhsd       10.0\n"
            ".double rhsd       1.5\n"
            "<main:\n"
            "    ldw r1 $0(gs)   ; Should be 96\n"
            "    ldw r2 $8(gs)   ; Should be 42\n"
            "    ldw r3 $16(gs)  ; Should be 55\n"
            "    ldw r4 $24(gs)  ; Should be 568888\n"
            "    ldw r9 $0(gs)   ; load integer into 49\n"
            "    add r9 r9 $1    ; add 1 to 96\n"
            "    ldw r9  $40(gs)       \n"
            "    ldw r10 $48(gs)      \n"
            "    add.d r0 r9 r10  ; 11.5\n"
            "    sub.d r0 r9 r10  ; 8.5\n"
            "    mul.d r0 r10 r9  ; 15.0\n"
            "    div.d r0 r9 r10  ; 6.66\n"
            ">\n";

    struct TestCase
    {
        std::string data;
        std::string name;
        std::vector<TestInstructPairs> instructs;
    };

    std::vector<TestCase> TEST_CASES = {

        // Yield TESTS 
        TestCase 
        { 
            ASM_YIELD, "Yield Tests", 
            { 
                {TestInstructs::STEP,      4,  0},
                {TestInstructs::CHECK_REG, 0,  1},
                {TestInstructs::STEP,      3,  0},
                {TestInstructs::CHECK_REG, 0, 99},
                {TestInstructs::STEP,      3,  0},
                {TestInstructs::CHECK_REG, 0,  2},
                {TestInstructs::STEP,      3,  0},
            } 
        },

        // Load Store TESTS
        TestCase
        {
            ASM_LOAD_STORE, "Load Store Tests",
            {
                {TestInstructs::STEP,      1,   0},
                {TestInstructs::CHECK_REG, 0,  42},
                {TestInstructs::STEP,      1,   0},
                {TestInstructs::CHECK_REG, 1,  69},
                {TestInstructs::STEP,      3,   0},
                {TestInstructs::STEP,      1,   0},
                {TestInstructs::CHECK_REG, 0,  42},
                {TestInstructs::STEP,      1,   0},
                {TestInstructs::CHECK_REG, 1,  69}
            }
        },

        // STD Arithmetic TESTS
        TestCase
        {
            ASM_STD_ARITH, "Standard Arithmetic",
            {
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,   12},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,   24},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 1,   30},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,   40},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,    8},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,    0},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 1,   -6},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,   16},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,    4},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,   16},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 1,   32},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 1,  128},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,   20},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,    1},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 0,   50},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 1,   25},
                {TestInstructs::STEP,      1,    0},
                {TestInstructs::CHECK_REG, 1,    4},
            }
        },

        // DBL Arithmetic TESTS
        TestCase
        {
            ASM_DBL_ARITH, "Double-based Arithmetic",
            {
                // Step all of the loads - we aren't testing those here
                {TestInstructs::STEP,      9,  0},
                {TestInstructs::CHECK_REG, 0,  static_cast<int64_t>(TEST::doubleToUint64(11.5))},
                {TestInstructs::STEP,      1,  0},
                {TestInstructs::CHECK_REG, 0,  static_cast<int64_t>(TEST::doubleToUint64(8.5))},
                {TestInstructs::STEP,      1,  0},
                {TestInstructs::CHECK_REG, 0,  static_cast<int64_t>(TEST::doubleToUint64(15.0))},
                {TestInstructs::STEP,      1,  0},

                // To ensure full precision and not rely on double casting for checks we use the raw value here. (~6.666667)
                {TestInstructs::CHECK_REG, 0,  4619192017806338731},
            }
        },
    };

    bool generate_asm_file(TestCase tc)
    {
        // Write the TC to file
        {
            std::ofstream out (asm_file, std::ios::out);
            if(!out.is_open())
            {
                std::cerr << "There was an error opening file to dump assembly file meant to be parsed for "
                          << tc.name << std::endl;
                return false;
            }
            // Write test case to file for parsing
            out.write(&tc.data[0], tc.data.size());
            out.close();
        }

        // Load the asm file, and produce a bin file
        {
            std::vector<uint8_t> bytes;

            if(!ASSEMBLER::ParseAsm(asm_file, bytes, false))
            {
                std::cerr << "Solace failed to parse the asm" << std::endl;
                return false;
            }

            std::ofstream out(bin_file, std::ios::out | std::ios::binary);
            if(!out.is_open())
            {
                std::cerr << "Unable to open " << bin_file << " for writing" << std::endl;
                return false;
            }

            out.write(reinterpret_cast<const char*>(&bytes[0]), bytes.size());
            out.close();
        }

        return true;
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(NablaAutomatedExecutionTests)
{
    void setup()
    {
        MemoryLeakWarningPlugin::turnOffNewDeleteOverloads();
    }

    void teardown()
    {
        MemoryLeakWarningPlugin::turnOnNewDeleteOverloads();
    }
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(NablaAutomatedExecutionTests, asms)
{
    for(auto & tc : TEST_CASES)
    {
        // Generate the ASM for the test
        CHECK_TRUE(generate_asm_file(tc));

        // Make a loadable VM
        NABLA::VSYS::LoadableMachine vm;

        // Load the VM
        NABLA::VSYS::LoadableMachine::LoadResultCodes result = vm.loadFile(bin_file);

        // Make sure its all good
        CHECK_TRUE(result == NABLA::VSYS::LoadableMachine::LoadResultCodes::OKAY);

        // Execute the checks specific to the test case
        for(auto & inspair : tc.instructs)
        {
            // Depending on the given instruction, take a different action
            switch(inspair.instruct)
            {
                // Step the machine a given amount
                case TestInstructs::STEP:
                {
                    NABLA::VSYS::ExecutionReturns step_result = vm.step(inspair.data);
                    CHECK_TRUE(step_result == NABLA::VSYS::ExecutionReturns::OKAY);
                    break;
                }

                // Check the contents of a register
                case TestInstructs::CHECK_REG:
                {
                    // There is only one context for tests
                    NABLA::VSYS::ExecutionContext * ec = vm.getExecutionContext(0);

                    CHECK_FALSE(nullptr == ec);

                    CHECK_EQUAL(inspair.expected, ec->registers[inspair.data]);
                    break;
                }

                // Fail
                default:
                {
                    FAIL("Somehow hit an unknown test instruction????");
                    break;
                }
            }
        }
    }
}
