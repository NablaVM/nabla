#include "vm.h"     // Vm header
#include "vmrc.h"   // Return codes
#include "vmins.h"  // Instructions
#include "vmsettings.h"
#include "stack.h"
#include "stackrc.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

#define NABLA_VM_BYTES_PER_INSTRUCTION 8

//  The virtual machine
//
struct VM
{
    uint8_t id;

    uint8_t registers[16];

    NablaStack globalStack;

    NablaStack callStack;
};

typedef struct VM NVM;

// -----------------------------------------------------
//
// -----------------------------------------------------

NVM * vm_new()
{
    NVM * vm = (NVM*)malloc(sizeof(NVM));

    vm->id = 1;

    vm->globalStack = stack_new(NABLA_SETTINGS_GLOBAL_STACK_SIZE);
    vm->callStack   = stack_new(NABLA_SETTINGS_CALL_STACK_SIZE);

    return vm;
}

/*
    ---------------------------------------------------------------------------------

                                VM LOADING FUCNTIONS 

    ---------------------------------------------------------------------------------
*/

void load_numerical_constant(NVM* vm, FILE* file, uint8_t nBytes, int *result)
{
    /*
        To populate the stack frame we go from n-1 to and including 0
        This will allow us to populate a stack frame uint64_t properly
        by telling us how much we have to shift

        This is required because of the way that bytes are stored in the file.
    
        When loaded, an 8-byte object is placed into the byte file as such:

                    (val & 0xFF00000000000000) >> 56    :  Need to shift 7 * 8
                    (val & 0x00FF000000000000) >> 48    :  Need to shift 8 * 8
                    (val & 0x0000FF0000000000) >> 40    :       ... etc
                    (val & 0x000000FF00000000) >> 32    : 
                    (val & 0x00000000FF000000) >> 24    : 
                    (val & 0x0000000000FF0000) >> 16    : 
                    (val & 0x000000000000FF00) >> 8     : 
                    (val & 0x00000000000000FF) >> 0     : 

        This means we have to figure out how many spaces to shift based on
        how many bytes we are reading in 

    */
    assert(nBytes > 0);

    uint64_t stackValue = 0;

    // We take nBytes-1 to so we can shift the most significant
    // data first. Then dec n to get the next spot, and so on.
    // Thankfully we can shift by 0 and stay where we are on the last byte
    for(int n = (nBytes-1); n >= 0; n--)   
    {
        uint8_t currentByte = 0;

        if(1 == fread(&currentByte, 1, 1, file))
        {
            stackValue |= (uint64_t)currentByte << (n * 8);
        }
        else
        {
            *result = -1;
            return;
        }
    }

    int pushResult;
    stack_push(stackValue, vm->globalStack, &pushResult);

    if(pushResult != STACK_OKAY)
    {
        *result = -1;
        return;
    }

    *result = 0;

    printf("numerical constant loaded : %lu\n", stackValue);
}

// -----------------------------------------------------
//
// -----------------------------------------------------

void load_string_constant(NVM* vm, FILE* file, int *result)
{
    uint8_t strSize = 0;
    if(1 != fread(&strSize, 1, 1, file))
    {
        *result = -1;
        return;
    }

    int      shifter    = 7;
    uint64_t stackValue = 0;

    for(int i = 0; i < strSize; i++)
    {
        uint8_t currentChunk = 0;
        if(1 != fread(&currentChunk, 1, 1, file))
        {
            *result = -1;
            return;
        }

        stackValue |= (uint64_t)currentChunk << (shifter * 8);

        printf("%c", (char)currentChunk);

        shifter--;

        // If we are below 0 that means this stack value is full
        // we need to reset the shifter, and push the value onto the stack
        if(shifter < 0)
        {
            shifter = 7;
            
            int pushResult;
            stack_push(stackValue, vm->globalStack, &pushResult);

            if(pushResult != STACK_OKAY)
            {
                *result = -1;
                return;
            }

            // Reset the stack value
            stackValue = 0;
        }
    }

    *result = 0;

    printf("\n");
}

// -----------------------------------------------------
//
// -----------------------------------------------------

int load_constant(NVM* vm, FILE * file, int currentByte)
{
    uint8_t op = (currentByte & 0xFC);
    uint8_t id = (currentByte & 0x03);

    if(op == CONST_INT)
    {
        int result = 0;
        if(id == 0) { /* .int8  */ load_numerical_constant(vm, file, 1, &result); }
        if(id == 1) { /* .int16 */ load_numerical_constant(vm, file, 2, &result); }
        if(id == 2) { /* .int32 */ load_numerical_constant(vm, file, 4, &result); }
        if(id == 3) { /* .int64 */ load_numerical_constant(vm, file, 8, &result); }

        if(result != 0) { return -1; }
    }

    if(op == CONST_DBL)
    {
        // Load 8 bytes
        int result = 0;
        if(id == 3) { /* .int64 */ load_numerical_constant(vm, file, 8, &result); }

        if(result != 0) { return -1; }
    }

    if(op == CONST_STR)
    {
        int result = 0;
        load_string_constant(vm, file, &result);

        if(result != 0) { return -1; }
    }

    return 0;
}

int load_function(NVM* vm, FILE* file)
{
    printf("load_function\n");

    // When called the func_create ins already eaten.
    // Now we need to eat 7 to figure out how many instructions to pull in
    // Once complete, we eat all of thos instructions, shoving them in the function frame
    // After all instructions eaten, push teh function frame to whatever storage gets created for functions

    uint64_t insBytes = 0;
    for(int n = 6; n >= 0; n--)   
    {
        uint8_t currentByte = 0;
        if(1 == fread(&currentByte, 1, 1, file))
        {
            insBytes |= (uint64_t)currentByte << (n * 8);
        }
        else
        {
            return -1;
        }
    }

    // All instructions are 8 bytes, so the number of instructions
    // is the bytes divided by 8
    uint64_t insCount = insBytes/NABLA_VM_BYTES_PER_INSTRUCTION;

    printf("There are : %lu instructions in the function\n", insCount);

    // Read all of the instructions from the file
    for(uint64_t ins = 0; ins < insCount; ins++)
    {
        uint64_t currentIns = 0;
        for(int n = 7; n >= 0; n--)   
        {
            uint8_t currentByte = 0;
            if(1 == fread(&currentByte, 1, 1, file))
            {
                currentIns |= (uint64_t)currentByte << (n * 8);

                printf("0x%X ", currentByte);
            }
            else
            {
                return -1;
            }
        }
        printf("\n");
        // Store the instruction here
        //
        //

        #warning This is where work needs to continue. Create something to holster functions, then put all currentIns here
        
    }

    return 0;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

int vm_load_file(FILE* file, NVM * vm)
{
    if(vm == NULL) { return VM_ERROR_NULL_VM; }
    if(file == NULL){ return VM_ERROR_FILE_OPEN; }

    // Read in constants - Populate into global stack
    // Once complete, read in functions
    //  If function create found, read all instructions into function frame until function end found
    //  then store that frame into vm
    // Do ^ until all bytes read-in

    uint8_t currentByte;

    enum LoadStatus
    {
        CONSTANTS,
        EXPECT_FUCNTION
    };

    enum LoadStatus currentStatus = CONSTANTS;

    while (  fread(&currentByte, 1, 1, file)  )  
    {
        // Start off assuming that we are getting constants
        // The way that the solace parser works is it stuffs all constants first 
        // in the order that they are defined in the asm file 
        if(currentStatus == CONSTANTS)
        {
            // Chop off the bottom 2 bits to see what operation is indicated
            uint8_t op = (currentByte & 0xFC);

            // If the instruction is to load a constant, call the constant loaders that will
            // consume that constant up-until the next instruction
            if(op == CONST_INT || op == CONST_DBL || op == CONST_STR)
            {
                if( 0 != load_constant(vm, file, currentByte) )
                {
                    return VM_ERROR_FAILED_TO_LOAD_CONSTANTS;
                }
            }
            // Since constants are loose-leaf in the binary, we need to check
            // if the next instruction is a function creation to get us out of 
            // loading constants
            // We use currentByte here, not op
            else  if(currentByte == INS_FUNCTION_CREATE)
            {
                currentStatus = EXPECT_FUCNTION;

                if(0 != load_function(vm, file)) { return VM_ERROR_FAILED_TO_LOAD_FUCNTION; }

            }
        }

        // Now we are expecting to read in either function starts, or function ends.
        // If we see a start, we send off the file to have the instructions pulled out
        // Function ends aren't part of the encoded instruction count in the function
        // creation instruction so we catch that here 
        else if (currentStatus == EXPECT_FUCNTION)
        {
            if(currentByte == INS_FUNCTION_END)
            {
                // We don't do anything here right now, but we might in 
                // the future, and this helps with debugging
                printf("Caught end of function\n");
            }
            if(currentByte == INS_FUNCTION_CREATE)
            {
                if(0 != load_function(vm, file))
                { 
                    return VM_ERROR_FAILED_TO_LOAD_FUCNTION; 
                }
            }
        }

        // Really, everything should have been gobbled up by now. If not
        // there might be an error. So we indicate that something is happening
        // and we don't know what it is but we don't like it!
        else
        {
            return VM_ERROR_UNHANDLED_INSTRUCTION;
        }
    } 

    return 0;
}