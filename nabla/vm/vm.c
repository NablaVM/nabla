#include "vm.h"     // Vm header
#include "vmrc.h"   // Return codes
#include "vmins.h"  // Instructions
#include "vmsettings.h"
#include "stack.h"
#include "stackrc.h"
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>

static uint8_t  FILE_GLOBAL_INVOKED_VM_COUNT = 0;
static uint8_t  FILE_GLOBAL_IS_VM_LOADED     = 0;
static uint8_t  FILE_GLOBAL_IS_VM_RUNNING    = 0;

//  A function of instructions
//
struct FUNC
{
    uint64_t ip;             // Instruction pointer
    NablaStack instructions; // Instructions
    NablaStack localStack;   // Local stack for storage
};
typedef struct FUNC NFUNC;

//  The virtual machine
//
struct VM
{
    uint8_t id;             // Specific vm id

    uint64_t fp;            // Function pointer

    uint64_t entryAddress;  // Entry function address listed in binary

    int64_t registers[16];  // VM Registers

    NablaStack globalStack; // Shared 'global' stack

    NablaStack callStack;   // Call stack

    NFUNC * functions; 
};
typedef struct VM NVM;

/*
    ---------------------------------------------------------------------------------

                                VM OPERATIONS

    ---------------------------------------------------------------------------------
*/

NVM * vm_new()
{
    // We make this assertion so if someone tries to be clever they have to 
    assert(NABLA_SETTINGS_BYTES_PER_INS == 8);

    NVM * vm = (NVM*)malloc(sizeof(NVM));

    assert(vm);

    for(uint8_t i = 0; i < 16; i++)
    {
        vm->registers[i] = 0;
    }

    vm->globalStack = stack_new(NABLA_SETTINGS_GLOBAL_STACK_SIZE);
    vm->callStack   = stack_new(NABLA_SETTINGS_CALL_STACK_SIZE);

    assert(vm->globalStack);
    assert(vm->callStack);

    vm->functions = (NFUNC *)malloc(NABLA_SETTINGS_MAX_FUNCTIONS * sizeof(NFUNC));

    assert(vm->functions);

    FILE_GLOBAL_INVOKED_VM_COUNT++;

    vm->id = FILE_GLOBAL_INVOKED_VM_COUNT;

    vm->fp = 0; // Function pointer= 0;

    //  Setup function structures
    //
    for(int i = 0; i < NABLA_SETTINGS_MAX_FUNCTIONS; i++)
    {
        // Create storage for instructions
        vm->functions[i].instructions = stack_new(NABLA_SETTINGS_MAX_IN_PER_FUCNTION);

        // Create the function's local stack
        vm->functions[i].localStack   = stack_new(NABLA_SETTINGS_LOCAL_STACK_SIZE);

        // Make sure its all allocated
        assert(vm->functions[i].instructions);
        assert(vm->functions[i].localStack);

        // Set the instruction pointer 
        vm->functions[i].ip = 0;
    }

    return vm;
}

// -----------------------------------------------------
//  Extract a byte from a 64-bit instruction
// -----------------------------------------------------

uint8_t run_extract_one(uint64_t data, uint8_t idx)
{
    return (data >> (8*idx)) & 0xff;
}

// -----------------------------------------------------
//  Extract 2 bytes from a 64-bit instruction
// -----------------------------------------------------

uint16_t run_extract_two(uint64_t data, uint8_t idx)
{
    assert(idx > 0);
    return (data >> (8*(idx-1))) & 0xffff;
}

// -----------------------------------------------------
//  Build lhs and rhs values for an arithmatic operation
// -----------------------------------------------------

void run_get_arith_lhs_rhs(NVM * vm, uint8_t id, uint64_t ins, int64_t * lhs, int64_t * rhs)
{
    if(id == 0)
    {
        *lhs =   vm->registers[run_extract_one(ins, 5)];
        *rhs =   vm->registers[run_extract_one(ins, 4)];
    }
    else if (id == 1)
    {
        *lhs =  vm->registers[run_extract_one(ins, 5)];
        *rhs =  run_extract_two(ins, 4);
    }
    else if (id == 2)
    {
        *lhs =  run_extract_two(ins, 5);
        *rhs =  vm->registers[run_extract_one(ins, 3)];
    }
    else if (id == 3)
    {
        *lhs =  run_extract_two(ins, 5);
        *rhs =  run_extract_two(ins, 3);
    }
}


// -----------------------------------------------------
//
// -----------------------------------------------------

int vm_run(NVM* vm)
{
    // Ensure vm is okay, check that its loaded, and not running
    assert(vm);
    if(0 == FILE_GLOBAL_IS_VM_LOADED)  {  return VM_RUN_ERROR_VM_NOT_LOADED;      }
    if(1 == FILE_GLOBAL_IS_VM_RUNNING) {  return VM_RUN_ERROR_VM_ALREADY_RUNNING; }

    // Indicate that it is now running
    FILE_GLOBAL_IS_VM_RUNNING = 1;


#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("Running VM\n");
#endif

    // Set function pointer to the entry function
    vm->fp = vm->entryAddress;

    NFUNC * currentFunction = &vm->functions[vm->fp];
    currentFunction->ip = 0;

    while(FILE_GLOBAL_IS_VM_RUNNING)
    {
        int res = 0;
        uint64_t ins = stack_value_at(currentFunction->ip, currentFunction->instructions, &res);

        if(res != STACK_OKAY)
        {
            printf("DEBUG: INSTRUCTION NOT FOUND FROM IP");
            return VM_RUN_ERROR_INSTRUCTION_NOT_FOUND;
        }

        uint8_t operation =  run_extract_one(ins, 7);
        uint8_t op = (operation & 0xFC);
        uint8_t id = (operation & 0x03);


        printf("Operation  :  %u\n", operation);
        printf("op         :  %u\n", op);
        printf("id         :  %u\n", id);

        int64_t lhs = 0;
        int64_t rhs = 0;

#warning before continuing on here I want to find a way to abstract data types ints / floats / doubles / strs / etc all for stack reg storage and arith operations

        switch(op)
        {
            case INS_ADD  :
            {
                uint8_t dest =  run_extract_one(ins, 6);
                assert(dest < 16);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = lhs+rhs;

                printf("result: %ld\n", vm->registers[dest]);
                break;
            }          
            case INS_SUB  :
            {
                uint8_t dest =  run_extract_one(ins, 6);
                assert(dest < 16);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = lhs-rhs;

                printf("result: %ld\n", vm->registers[dest]);

                if ( vm->registers[dest] >> 63 & 1) printf ("value is negative\n");
                break;
            }          
            case INS_MUL  :
            {
                uint8_t dest =  run_extract_one(ins, 6);
                assert(dest < 16);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = lhs*rhs;

                printf("result: %ld\n", vm->registers[dest]);
                break;
            }          
            case INS_DIV  :
            {
                uint8_t dest =  run_extract_one(ins, 6);
                assert(dest < 16);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);

                if(rhs == 0){ vm->registers[dest] = 0; }
                else { vm->registers[dest] = lhs/rhs; }
                
                printf("result: %ld\n", vm->registers[dest]);
                break;
            }          
            case INS_ADDD :
            {
                break;
            }          
            case INS_SUBD :
            {
                break;
            }          
            case INS_MULD :
            {
                break;
            }          
            case INS_DIVD :
            {
                break;
            }          
            case INS_BGT  :
            {
                break;
            }          
            case INS_BGTE :
            {
                break;
            }          
            case INS_BLT  :
            {
                break;
            }          
            case INS_BLTE :
            {
                break;
            }          
            case INS_BEQ  :
            {
                break;
            }          
            case INS_BNE  :
            {
                break;
            }          
            case INS_BGTD :
            {
                break;
            }          
            case INS_BGTED:
            {
                break;
            }          
            case INS_BLTD :
            {
                break;
            }          
            case INS_BLTED:
            {
                break;
            }          
            case INS_BEQD :
            {
                break;
            }          
            case INS_BNED :
            {
                break;
            }          
            case INS_MOV  :
            {
                break;
            }          
            case INS_LDA  :
            {
                break;
            }          
            case INS_LDB  :
            {
                break;
            }          
            case INS_STB  :
            {
                break;
            }          
            case INS_PUSH :
            {
                break;
            }          
            case INS_POP  :
            {
                break;
            }          
            case INS_JUMP :
            {
                break;
            }          
            case INS_CALL :
            {
                break;
            }          
            case INS_RET  :
            {
                break;
            }          
            case INS_EXIT :
            {
                FILE_GLOBAL_IS_VM_RUNNING = 0;
                return 0;
            }         
            default:
            {
                uint64_t stackEnd = stack_get_size(currentFunction->instructions);
                if(currentFunction->ip == stackEnd)
                {
                    //  This will need to be updated in the future to check the call stack and check
                    //  for return instructions. If there isn't any THEN this should happen. For now, we die
                    //
                    FILE_GLOBAL_IS_VM_RUNNING = 0;
                    return 0;
                }

                return VM_RUN_ERROR_UNKNOWN_INSTRUCTION;
                break; 
            }
        }

        //  Increase the instruction pointer
        //
        currentFunction->ip++;
    }

    return 0;
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

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("numerical constant loaded : %lu\n", stackValue);
#endif
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

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
        printf("%c", (char)currentChunk);
#endif
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

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("\n");
#endif
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

    if(currentByte == CONST_DBL)
    {
        // Load 8 bytes
        int result = 0;
        load_numerical_constant(vm, file, 8, &result);

        if(result != 0) { return -1; }
    }

    if(currentByte == CONST_STR)
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

int load_end_of_binary(NVM* vm, FILE * file)
{
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("load_end_of_binary\n");
#endif

    // Need to eat the entire eof instruction
    for(int c = 0; c < 7; c++)   
    {
        uint8_t tmp = 0;
        if(1 != fread(&tmp, 1, 1, file))
        {
            return -1;
        }
    }

    return 0;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

int load_function(NVM* vm, FILE* file)
{
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("load_function\n");
#endif

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
    uint64_t insCount = insBytes/NABLA_SETTINGS_BYTES_PER_INS;

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("There are : %lu instructions in the function\n", insCount);
#endif

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

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("0x%X ", currentByte);
#endif
            }
            else
            {
                return -1;
            }
        }

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
        printf("\n");
#endif
        
        int pushResult;
        stack_push(currentIns, vm->functions[vm->fp].instructions, &pushResult);

        if(pushResult != STACK_OKAY)
        {
            return -1;
        }        
    }

    // Now that the instructions are loaded, we increase the function pointer as we are using it to access
    // the function storage for loading
    vm->fp++;

    return 0;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

int vm_load_file(FILE* file, NVM * vm)
{
    if(vm == NULL) { return VM_LOAD_ERROR_NULL_VM; }
    if(file == NULL){ return VM_LOAD_ERROR_FILE_OPEN; }

    if(FILE_GLOBAL_IS_VM_LOADED == 1)
    {
        return VM_LOAD_ERROR_ALREADY_LOADED;
    }

    // Read in constants - Populate into global stack
    // Once complete, read in functions
    //  If function create found, read all instructions into function frame until function end found
    //  then store that frame into vm
    // Do ^ until all bytes read-in

    uint8_t currentByte;

    enum LoadStatus
    {
        IDLE,
        EXPECT_CONSTANT,
        EXPECT_FUCNTION,
        STOP_READING
    };

    enum LoadStatus currentStatus = IDLE;

    uint64_t numberOfConstants      = 0;
    uint64_t expectedEntryAddress   = 0;

    while ( fread(&currentByte, 1, 1, file) && currentStatus != STOP_READING )  
    {
        // From idle we can switch into expecting constants
        // or expecting functions. 
        if(currentStatus == IDLE)
        {
            if(currentByte == INS_SEG_CONST)
            {
                currentStatus = EXPECT_CONSTANT;

                // Read in constant count. Once we read that many constants in, 
                // switch over to IDLE
                for(int n = 7; n >= 0; n--)   
                {
                    uint8_t tmp = 0;
                    if(1 == fread(&tmp, 1, 1, file))
                    {
                        numberOfConstants |= (uint64_t)tmp << (n * 8);
                    }
                    else
                    {
                        return VM_LOAD_ERROR_FAILED_TO_LOAD_CONSTANTS;
                    }
                }
            }
            else if (currentByte == INS_SEG_FUNC)
            {
                currentStatus = EXPECT_FUCNTION;

                //  Read in the entry address expected 
                //
                for(int n = 7; n >= 0; n--)   
                {
                    uint8_t tmp = 0;
                    if(1 == fread(&tmp, 1, 1, file))
                    {
                        expectedEntryAddress |= (uint64_t)tmp << (n * 8);
                    }
                    else
                    {
                        return VM_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION;
                    }
                }
            }
            else
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Unhandled instruction while load-state IDLE\n");
#endif
            return VM_LOAD_ERROR_UNHANDLED_INSTRUCTION;
            }
        }

        // Start off assuming that we are getting constants
        // The way that the solace parser works is it stuffs all constants first 
        // in the order that they are defined in the asm file 
        else if(currentStatus == EXPECT_CONSTANT)
        {
            printf("Expecting %lu constants\n", numberOfConstants);

            // Chop off the bottom 2 bits to see what operation is indicated
            uint8_t op = (currentByte & 0xFC);

            // If the instruction is to load a constant, call the constant loaders that will
            // consume that constant up-until the next instruction
            if(op == CONST_INT || currentByte == CONST_DBL || currentByte == CONST_STR)
            {
                if( 0 != load_constant(vm, file, currentByte) )
                {
                    return VM_LOAD_ERROR_FAILED_TO_LOAD_CONSTANTS;
                }

                // Dec the number of expected constants
                numberOfConstants--;
            }
            else
            {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Unhandled instruction while load-state EXPECT_CONSTANT\n");
#endif
                return VM_LOAD_ERROR_UNHANDLED_INSTRUCTION;
            }

            //  All functions have been found, time to switch to IDLE
            //
            if(numberOfConstants == 0)
            {
                currentStatus = IDLE;
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
                
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Caught end of function\n");
#endif
                // Need to eat the entire eof instruction
                for(int c = 0; c < 7; c++)   
                {
                    uint8_t tmp = 0;
                    if(1 != fread(&tmp, 1, 1, file))
                    {
                        return VM_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION;
                    }
                }
            }
            else if(currentByte == INS_FUNCTION_CREATE)
            {
                if(0 != load_function(vm, file))
                { 
                    return VM_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION; 
                }
            }
            else if (currentByte == INS_SEG_BEOF)
            {
                if(0 != load_end_of_binary(vm, file))
                {
                    return VM_LOAD_ERROR_FAILED_TO_LOAD_FUCNTION; 
                }

                // Set the state to end. 
                currentStatus = STOP_READING;
            }
            else
            {

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Unhandled instruction while load-state EXPECT_FUCNTION : %X \n", currentByte);
#endif
                return VM_LOAD_ERROR_UNHANDLED_INSTRUCTION;
            }
        }
    } 

    //  Ensure that we stopped reading because we wanted to
    //
    if(currentStatus != STOP_READING)
    {
        return VM_LOAD_ERROR_EOB_NOT_FOUND;
    }

    assert(expectedEntryAddress <= NABLA_SETTINGS_MAX_FUNCTIONS);

    //  Set the entry address 
    //
    vm->entryAddress = expectedEntryAddress;

    //  Indicate that the vm is loaded
    //
    FILE_GLOBAL_IS_VM_LOADED = 1;
    return 0;
}