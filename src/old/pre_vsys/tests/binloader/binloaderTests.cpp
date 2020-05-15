/*
    These tests could easily and accidentally become what the vm tests are trying to accomplish, so read this:
    These tests are meant to ensure that the loading of a binary is correct, that is, the resulting vm begins in a start 
    state that we expect. In some cases we might step the vm to aid the test, but ensuring correct execution of instructions
    for the vm is NOT the goal here. That is the goal of the vm tests.
*/

extern "C"
{
    #include "vm.h"
    #include "VmInstructions.h"
    #include "binloader.h"
    #include "util.h"
}

#include "bytegen.hpp"
#include "assembler.hpp"
#include "testdata.hpp"

#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include "CppUTest/TestHarness.h"

namespace
{
#ifdef TARGET_PLATFORM_IS_WINDOWS
    const std::string asm_file = "tmp_binloader_test_file.asm";
    const std::string bin_file = "tmp_binloader_test_binary.out";
#else 
    const std::string asm_file = "/tmp/binloader_test_file.asm";
    const std::string bin_file = "/tmp/binloader_test_binary.out";
#endif
    //  Tests for binloader require a bin file. Here we use solace to assemble a binary, write it out to
    //  bin_file location and return true if all goes well. Then the test can carry on loading the binary
    //  and ensuring that the vm is populated correctly
    bool parseAndWrite(std::string ASM)
    {
        // Write the asm file
        {
            std::ofstream out(asm_file, std::ios::out);
            if(!out.is_open())
            {
                std::cerr << "There was an error opening file to dump assembly file meant to be parsed" << std::endl;
                return false;
            }
            out.write(&ASM[0], ASM.size());
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

    // ---------------------------------------------------------------
    // 
    // ---------------------------------------------------------------

    bool load_vm(NablaVirtualMachine vm)
    {
        FILE * file_in = fopen(bin_file.c_str(), "rb");

        if(file_in == NULL)
        {
            std::cerr << "load_vm : Unable to open file " << bin_file << std::endl;
            return false;
        }
        
        // Hand the file and vm over to be populated
        switch(bin_load_vm(file_in, vm))
        {
            case BIN_LOAD_ERROR_NULL_VM:                    fclose(file_in); return false;
            case BIN_LOAD_ERROR_FILE_OPEN:                  fclose(file_in); return false;
            case BIN_LOAD_ERROR_FAILED_TO_LOAD_CONSTANTS:   fclose(file_in); return false;
            case BIN_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION:    fclose(file_in); return false;
            case BIN_LOAD_ERROR_UNHANDLED_INSTRUCTION:      fclose(file_in); return false;
            case BIN_LOAD_ERROR_ALREADY_LOADED:             fclose(file_in); return false;
            case BIN_LOAD_ERROR_EOB_NOT_FOUND:              fclose(file_in); return false;
            default:                                        fclose(file_in);  break;
        }

        // Allow us to re-use binloader in the same application.
        bin_reset();
        return true;
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST_GROUP(BinloaderTests)
{   
    // We disable memory stuff here because there are globally scoped things 
    // in the assembler that causes cpputest to freak out because the memory
    // isn't freed before the test exists. 
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
//  Load a file with some constants. and make sure that the loader put them in correctly
// ---------------------------------------------------------------

TEST(BinloaderTests, asmConstants)
{
    // Write the file we want to binload
    CHECK_TRUE_TEXT(parseAndWrite(NABLA::TEST::BINLOADER::ASM_CONSTANTS), "Unable to generate ASM_CONSTANTS program");

    NablaVirtualMachine vm;

    // Create the virtual machine
    vm = vm_new();

    // Ensure it was created
    CHECK_FALSE_TEXT(nullptr == vm, "VM Came back NULL");

    // Use binloader to populate the VM
    CHECK_TRUE_TEXT(load_vm(vm), "Binloader failed to populate VM");

    // Analyze VM to ensure that binloader did what we expected

    // Init
    vm_init(vm);

    // Execute the 'size' instruction
    vm_step(vm, 1);

    // Ensure that the 'size' instruction has put the correct value into reg 0
    CHECK_EQUAL(8, vm->registers[0]);

    int result = -255;

    CHECK_EQUAL_TEXT(96, stack_value_at(0, vm->globalStack, &result), "Value in gs 0 not what we expected"); 
    CHECK_EQUAL_TEXT(STACK_OKAY, result, "Getting stack value failed");

    CHECK_EQUAL_TEXT(42, stack_value_at(1, vm->globalStack, &result), "Value in gs 1 not what we expected"); 
    CHECK_EQUAL_TEXT(STACK_OKAY, result, "Getting stack value failed");

    CHECK_EQUAL_TEXT(55, stack_value_at(2, vm->globalStack, &result), "Value in gs 2 not what we expected"); 
    CHECK_EQUAL_TEXT(STACK_OKAY, result, "Getting stack value failed");

    CHECK_EQUAL_TEXT(568888, stack_value_at(3, vm->globalStack, &result), "Value in gs 3 not what we expected"); 
    CHECK_EQUAL_TEXT(STACK_OKAY, result, "Getting stack value failed");

    CHECK_EQUAL_TEXT(util_convert_double_to_uint64(10.0), stack_value_at(4, vm->globalStack, &result), "Value in gs 4 not what we expected"); 
    CHECK_EQUAL_TEXT(STACK_OKAY, result, "Getting stack value failed");

    CHECK_EQUAL_TEXT(util_convert_double_to_uint64(1.5), stack_value_at(5, vm->globalStack, &result), "Value in gs 5 not what we expected"); 
    CHECK_EQUAL_TEXT(STACK_OKAY, result, "Getting stack value failed");

    // The next pieces are encoded strings and look really weird, but don't worry.
    CHECK_EQUAL_TEXT(6082227239949792032, stack_value_at(6, vm->globalStack, &result), "Value in gs 6 not what we expected"); 
    CHECK_EQUAL_TEXT(STACK_OKAY, result, "Getting stack value failed");

    CHECK_EQUAL_TEXT(6998720764906663527, stack_value_at(7, vm->globalStack, &result), "Value in gs 7 not what we expected"); 
    CHECK_EQUAL_TEXT(STACK_OKAY, result, "Getting stack value failed");

    vm_delete(vm);
}

// ---------------------------------------------------------------
//  Load a file with some functions. and make sure that the loader put them in correctly
// ---------------------------------------------------------------

TEST(BinloaderTests, asmFunctions)
{
    // Write the file we want to binload
    CHECK_TRUE_TEXT(parseAndWrite(NABLA::TEST::BINLOADER::ASM_FUNCTIONS), "Unable to generate ASM_FUNCTIONS program");

    NablaVirtualMachine vm;

    // Create the virtual machine
    vm = vm_new();

    // Ensure it was created
    CHECK_FALSE_TEXT(nullptr == vm, "VM Came back NULL");

    // Use binloader to populate the VM
    CHECK_TRUE_TEXT(load_vm(vm), "Binloader failed to populate VM");

    // Analyze VM to ensure that binloader did what we expected

    // Init
    vm_init(vm);

    CHECK_FALSE_TEXT((nullptr == vm->functions), "VM Functions pointer was null. How?");
    CHECK_EQUAL_TEXT(2, vm->entryAddress, "The VM entry address wasn't correct");
    CHECK_EQUAL_TEXT(2, vm->fp, "VM Function pointer was not set to entry point");
    CHECK_EQUAL_TEXT(3, vm->numberOfFunctions, "Unexpected number of functions");
    CHECK_EQUAL_TEXT(0, vm->functions[vm->fp].ip, "The entry function pointer was not set to 0");

    vm_delete(vm);
}

// ---------------------------------------------------------------
//  Load a file with some functions and instructs. make sure that the loader put them in correctly
// ---------------------------------------------------------------

TEST(BinloaderTests, asmInstructs)
{
    /*
        As far as instructions are concerned and the VM all instructions are created and loaded equally. No matter
        what instruction it is, it should be loaded the same.. with the exception of 'call' Call is a wild animal
        that generates a couple of extra helper functions. We make sure that standard instructions, and call instructions
        don't have any abberant loads here. We also ensure that labels don't get interpreted as instructions
        (even though that would technically be a solace issue)
    */

    // Write the file we want to binload
    CHECK_TRUE_TEXT(parseAndWrite(NABLA::TEST::BINLOADER::ASM_INSTRUCTS), "Unable to generate ASM_INSTRUCTS program");

    NablaVirtualMachine vm;

    // Create the virtual machine
    vm = vm_new();

    // Ensure it was created
    CHECK_FALSE_TEXT(nullptr == vm, "VM Came back NULL");

    // Use binloader to populate the VM
    CHECK_TRUE_TEXT(load_vm(vm), "Binloader failed to populate VM");

    // Analyze VM to ensure that binloader did what we expected

    // Init
    vm_init(vm);

    // There should be 8 instructions, as the 'call' instruction actually requres 3 'forbidden' instructions that are generated
    // by solace and written to file without anyone knowing. spooky.
    CHECK_EQUAL_TEXT(8, stack_get_size(vm->functions[0].instructions), "Invalid number of instructions loaded into function");

    // Ensure that the label-filled instructionless function doesn't have any actual instructions in it.
    CHECK_EQUAL_TEXT(0, stack_get_size(vm->functions[1].instructions), "Invalid number of instructions loaded into function");

    vm_delete(vm);
}