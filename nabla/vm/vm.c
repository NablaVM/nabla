#include "vm.h"     // Vm header
#include "vmrc.h"   // Return codes
#include "vmins.h"  // Instructions
#include "vmsettings.h"
#include "stack.h"
#include "stackrc.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

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

// -----------------------------------------------------
//
// -----------------------------------------------------

void load_numerical_constant(NVM* vm, FILE* file, uint8_t nBytes, int *result)
{
    printf("constant_smart_load\n");

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


    while (  fread(&currentByte, 1, 1, file)  )  
    {
        uint8_t op = (currentByte & 0xFC);

        if(op == CONST_INT || op == CONST_DBL || op == CONST_STR)
        {
            uint8_t intType = (currentByte & 0x03);

            if( 0 != load_constant(vm, file, currentByte) )
            {
                return VM_ERROR_FAILED_TO_LOAD_CONSTANTS;
            }
        }
    } 

    return 0;
}