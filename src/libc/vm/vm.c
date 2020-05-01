/*
    Josh A. Bosley 2020

    Function naming:
        vm_*    functions are able to be accessed outside of this source 
        run_*   functions are used internally during the 'running' phase of operation
*/

#include "vm.h"     // Vm header
#include "VmInstructions.h"

#include "io.h"     // IO Device
#include "net.h"    // Network Device
#include "util.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct FUNC NFUNC;
typedef struct VM NVM;


static uint8_t   FILE_GLOBAL_INVOKED_VM_COUNT  = 0;
static uint8_t   FILE_GLOBAL_IS_VM_RUNNING     = 0;
static uint8_t   FILE_GLOBAL_IS_VM_INITIALIZED = 0;

// The current function to get instructions from
static NFUNC * currentFunction;

// The IO Device
static struct IODevice * io_device;

// Indicate if we are switching functions. When this is set, we don't want to increase the instruction pointer
// as we have modified it as-per the guidance of an instruction. Either a call, or a return. In any case we want
// to ensure the bottom of the loop doesn't increase the ip
static uint8_t switchingFunction;

/*
    ---------------------------------------------------------------------------------

                                VM OPERATIONS

    ---------------------------------------------------------------------------------
*/

NVM * vm_new()
{
    // We make this assertion so if someone tries to be clever they have to 
    assert(VM_SETTINGS_BYTES_PER_INS == 8);

    NVM * vm = (NVM*)malloc(sizeof(NVM));

    assert(vm);

    for(uint8_t i = 0; i < 16; i++)
    {
        vm->registers[i] = 0;
    }

    vm->globalStack = stack_new(VM_SETTINGS_GLOBAL_STACK_SIZE);
    vm->callStack   = stack_new(VM_SETTINGS_CALL_STACK_SIZE);

    assert(vm->globalStack);
    assert(vm->callStack);

    vm->functions = (NFUNC *)malloc(VM_SETTINGS_MAX_FUNCTIONS * sizeof(NFUNC));

    assert(vm->functions);

    io_device = io_new();
    assert(io_device);

    FILE_GLOBAL_INVOKED_VM_COUNT++;

    vm->id = FILE_GLOBAL_INVOKED_VM_COUNT;

    vm->fp = 0; // Function pointer= 0;

    //  Setup function structures
    //
    for(int i = 0; i <VM_SETTINGS_MAX_FUNCTIONS; i++)
    {
        // Create storage for instructions
        vm->functions[i].instructions = stack_new(VM_SETTINGS_MAX_IN_PER_FUCNTION);

        // Create the function's local stack
        vm->functions[i].localStack   = stack_new(VM_SETTINGS_LOCAL_STACK_SIZE);

        // Make sure its all allocated
        assert(vm->functions[i].instructions);
        assert(vm->functions[i].localStack);

        // Set the instruction pointer 
        vm->functions[i].ip = 0;
    }

    return vm;
}

// -----------------------------------------------------
// 
// -----------------------------------------------------

void vm_delete(NVM * vm)
{
    FILE_GLOBAL_IS_VM_RUNNING     = 0;
    FILE_GLOBAL_IS_VM_INITIALIZED = 0;
    for(int i = 0; i <VM_SETTINGS_MAX_FUNCTIONS; i++)
    {
        stack_delete(vm->functions[i].instructions);
        stack_delete(vm->functions[i].localStack);
    }
    free(vm->functions);
    stack_delete(vm->globalStack);
    stack_delete(vm->callStack);
    io_delete(io_device);
    free(vm);

    // Should be, but lets be careful
    if(FILE_GLOBAL_INVOKED_VM_COUNT > 0)
    {
        FILE_GLOBAL_INVOKED_VM_COUNT--;
    }
}

// -----------------------------------------------------
//  Build lhs and rhs values for an arithmatic operation
// -----------------------------------------------------

void run_get_arith_lhs_rhs(NVM * vm, uint8_t id, uint64_t ins, int64_t * lhs, int64_t * rhs)
{
    if(id == 0)
    {
        *lhs =   vm->registers[util_extract_byte(ins, 5)];
        *rhs =   vm->registers[util_extract_byte(ins, 4)];
    }
    else if (id == 1)
    {
        *lhs =  vm->registers[util_extract_byte(ins, 5)];
        *rhs = util_extract_two_bytes(ins, 4);
    }
    else if (id == 2)
    {
        *lhs =  util_extract_two_bytes(ins, 5);
        *rhs =  vm->registers[util_extract_byte(ins, 3)];
    }
    else if (id == 3)
    {
        *lhs =  util_extract_two_bytes(ins, 5);
        *rhs =  util_extract_two_bytes(ins, 3);
    }
}

// -----------------------------------------------------
//
// -----------------------------------------------------

int vm_init(struct VM* vm)
{
    assert(vm);

    if(FILE_GLOBAL_IS_VM_INITIALIZED)
    {
        perror("VM is already initialized\n");
        return VM_INIT_ERROR_ALREADY_INITIALIZED;
    }

    // Set function pointer to the entry function
    vm->fp = vm->entryAddress;

    // The current function to get instructions from
    currentFunction = &vm->functions[vm->fp];

    // Indicate if we are switching functions. When this is set, we don't want to increase the instruction pointer
    // as we have modified it as-per the guidance of an instruction. Either a call, or a return. In any case we want
    // to ensure the bottom of the loop doesn't increase the ip
    switchingFunction = 0;

    // The current function's instruction pointer ( the instruction we want to fetch, decode, and execute )
    currentFunction->ip = 0;

    FILE_GLOBAL_IS_VM_INITIALIZED = 1;

    return 0;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

int vm_cycle(struct VM* vm, uint64_t n)
{
    for(uint64_t cycle = 0; cycle < n; cycle++)
    {
        int res = 0;
        uint64_t ins = stack_value_at(currentFunction->ip, currentFunction->instructions, &res);

        if(res != STACK_OKAY)
        {
            /*
                If the result isn't okay we attempt to return. This is because we are out of instructions 
                and the developer decided to not use the 'ret' keyword because who does if you can go without?

                PEOPLE WHO LIKE OPTIMIZED CODE! If you use the 'ret' keyword or 'exit' in main, 
                then this sort of ugly doesn't have to happen
            */
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("Current instruction stack empty, attempting to force a return.\n");
#endif
            goto vm_attempt_force_return;
        }

        // The full first byte of the instruction
        uint8_t operation =  util_extract_byte(ins, 7);

        // The 'opcode' of the instruction (first 6 bits)
        uint8_t op = (operation & 0xFC);

        // The 'id' of the isntruction (key information that tells us how to decode the rest of the instruction)
        uint8_t id = (operation & 0x03);

        // Left hand side of an operation (arg 2 of asm instruction)
        int64_t lhs = 0;

        // Right hand side of an operation (arg 3 of an asm instruction)
        int64_t rhs = 0;

        switch(op)
        {     
            case INS_NOP:
            {
                // No op
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("nop\n");
#endif
                break;
            }
            case INS_LSH :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = (lhs << rhs);

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("LSH : result: %ld\n", vm->registers[dest]);
#endif
                break;
            } 

            case INS_RSH :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = (lhs >> rhs);
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("RSH : result: %ld\n", vm->registers[dest]);
#endif
                break;
            } 

            case INS_AND :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = (lhs & rhs);
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("AND : result: %ld\n", vm->registers[dest]);
#endif
                break;
            } 

            case INS_OR  :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = (lhs | rhs);
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("OR : result: %ld\n", vm->registers[dest]);
#endif
                break;
            } 

            case INS_XOR :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = (lhs ^ rhs);
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("XOR : result: %ld\n", vm->registers[dest]);
#endif
                break;
            } 

            case INS_NOT :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = (~lhs);

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("NOT : result: %ld\n", vm->registers[dest]);
#endif
                break;
            } 

            case INS_ADD  :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                assert(dest < 16);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = lhs+rhs;

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("result: %ld\n", vm->registers[dest]);
#endif
                break;
            }          
            case INS_SUB  :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                assert(dest < 16);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = lhs-rhs;

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("result: %ld\n", vm->registers[dest]);
#endif
                break;
            }          
            case INS_MUL  :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                assert(dest < 16);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);
                vm->registers[dest] = lhs*rhs;

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("result: %ld\n", vm->registers[dest]);
#endif
                break;
            }          
            case INS_DIV  :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                assert(dest < 16);
                run_get_arith_lhs_rhs(vm, id, ins, &lhs, &rhs);

                if(rhs == 0){ vm->registers[dest] = 0; }
                else { vm->registers[dest] = lhs/rhs; }
                
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("result: %ld\n", vm->registers[dest]);
#endif
                break;
            }          
            case INS_ADDD :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                assert(dest < 16);

                run_get_arith_lhs_rhs(vm, 0, ins, &lhs, &rhs);
            
                vm->registers[dest] = util_convert_double_to_uint64( util_convert_uint64_to_double(lhs) + util_convert_uint64_to_double(rhs));

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Result : %f\n", util_convert_uint64_to_double(lhs) + util_convert_uint64_to_double(rhs));
#endif
                break;
            }          
            case INS_SUBD :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                assert(dest < 16);

                run_get_arith_lhs_rhs(vm, 0, ins, &lhs, &rhs);
            
                vm->registers[dest] = util_convert_double_to_uint64( util_convert_uint64_to_double(lhs) - util_convert_uint64_to_double(rhs));

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Result : %f\n", util_convert_uint64_to_double(lhs) - util_convert_uint64_to_double(rhs));
#endif
                break;
            }          
            case INS_MULD :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                assert(dest < 16);

                run_get_arith_lhs_rhs(vm, 0, ins, &lhs, &rhs);
            
                vm->registers[dest] = util_convert_double_to_uint64( util_convert_uint64_to_double(lhs) * util_convert_uint64_to_double(rhs));

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Result : %f\n", util_convert_uint64_to_double(lhs) * util_convert_uint64_to_double(rhs));
#endif
                break;
            }          
            case INS_DIVD :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                assert(dest < 16);

                run_get_arith_lhs_rhs(vm, 0, ins, &lhs, &rhs);
            
                vm->registers[dest] = util_convert_double_to_uint64( util_convert_uint64_to_double(lhs) / util_convert_uint64_to_double(rhs));

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Result : %f\n", util_convert_uint64_to_double(lhs) / util_convert_uint64_to_double(rhs));
#endif
                break;
            }          
            case INS_BGT  :
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);
                
                if(lhs > rhs){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BGTE :
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);
                
                if(lhs >= rhs){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BLT  :
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);
                
                if(lhs < rhs){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BLTE :
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);
                
                if(lhs <= rhs){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BEQ  :
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);

                if(lhs == rhs){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BNE  :
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);

                if(lhs != rhs){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BGTD :
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                double lhs_d = util_convert_uint64_to_double(lhs);
                double rhs_d = util_convert_uint64_to_double(rhs);

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);

                if(lhs_d > rhs_d){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BGTED:
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                double lhs_d = util_convert_uint64_to_double(lhs);
                double rhs_d = util_convert_uint64_to_double(rhs);

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);

                if(lhs_d >= rhs_d){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BLTD :
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                double lhs_d = util_convert_uint64_to_double(lhs);
                double rhs_d = util_convert_uint64_to_double(rhs);

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);

                if(lhs_d < rhs_d){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BLTED:
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                double lhs_d = util_convert_uint64_to_double(lhs);
                double rhs_d = util_convert_uint64_to_double(rhs);

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);

                if(lhs_d <= rhs_d){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BEQD :
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                double lhs_d = util_convert_uint64_to_double(lhs);
                double rhs_d = util_convert_uint64_to_double(rhs);

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);

                if(util_check_double_equal(lhs_d, rhs_d)){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_BNED :
            {
                lhs = vm->registers[util_extract_byte(ins, 6)];
                rhs = vm->registers[util_extract_byte(ins, 5)];

                double lhs_d = util_convert_uint64_to_double(lhs);
                double rhs_d = util_convert_uint64_to_double(rhs);

                uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                      (uint64_t)util_extract_two_bytes(ins, 2);

                if(!util_check_double_equal(lhs_d, rhs_d)){ vm->functions[vm->fp].ip = branchAddr; continue; }
                break;
            }          
            case INS_MOV  :
            {
                lhs = util_extract_byte(ins, 6);

                // Move register value into another register
                if(id == 0)
                {
                    rhs = util_extract_byte(ins, 5);
                    vm->registers[lhs] = vm->registers[rhs];
                }
                // Move numerival value into a register
                else if (id == 1)
                {
                    int8_t rval = util_extract_byte(ins, 5);
                    vm->registers[lhs] =  rval;
                }
                else
                {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Error: Unknown ID bits expressed in INS_MOV\n");
#endif
                    return VM_RUN_ERROR_UNKNOWN_INSTRUCTION;
                }

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("MOV result : %li\n", (int64_t)vm->registers[lhs]);
#endif

                break;
            }
            case INS_LDB  :
            {
                uint8_t dest =  util_extract_byte(ins, 6);
                uint8_t stackSouce = util_extract_byte(ins, 5);
                uint64_t sourceAddress;

                // Depending on the 'id' of the instruction we need to change where we are getting the value
                // for the stack offset.
                if(id == 0)
                {
                    sourceAddress = (uint64_t)util_extract_two_bytes(ins, 4) << 16| 
                                    (uint64_t)util_extract_two_bytes(ins, 2);

                }
                else if (id == 1)
                {
                    uint8_t sourceReg  = util_extract_byte(ins, 4);
                    sourceAddress = vm->registers[sourceReg];
                }
                else
                {
                    printf("Invalid 'ldb' instruction id : ID= %u\n", id);
                    return VM_RUN_ERROR_UNKNOWN_INSTRUCTION;
                }
                
                // Now that we have the source address and other required information, we can move on and actually do the load
                int okay = -255;
                if(stackSouce == GLOBAL_STACK)
                {
                    vm->registers[dest] = stack_value_at(sourceAddress, vm->globalStack, &okay);
                }
                else if ( stackSouce == LOCAL_STACK )
                {
                    vm->registers[dest] = stack_value_at(sourceAddress, vm->functions[vm->fp].localStack, &okay);
                }
                assert(okay == STACK_OKAY);

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Ldb Result: %lu\n", vm->registers[dest]);
#endif
                break;
            }          
            case INS_STB  :
            {
                uint8_t  stackDest = util_extract_byte(ins, 6);
                uint64_t destAddress;
                uint8_t  sourceReg;

                // Depending on the 'id' of the instruction we need to change where we are getting the value
                // for the stack offset.
                if(id == 0)
                {
                    sourceReg =  util_extract_byte(ins, 1);
                    destAddress = (uint64_t)util_extract_two_bytes(ins, 5) << 16| 
                                 (uint64_t)util_extract_two_bytes(ins, 3);
                }
                else if (id == 1)
                {
                    sourceReg = util_extract_byte(ins, 4);
                    uint8_t destReg   = util_extract_byte(ins, 5);
                    destAddress = vm->registers[destReg];
                }
                else
                {
                    printf("Invalid 'stb' instruction id : ID= %u\n", id);
                    return VM_RUN_ERROR_UNKNOWN_INSTRUCTION;
                }

                // Now that we have the destination information, perform the store
                int okay = -255;
                if(stackDest == GLOBAL_STACK)
                {
                    stack_set_value_at(destAddress, vm->registers[sourceReg], vm->globalStack, &okay);
                }
                else if ( stackDest == LOCAL_STACK )
                {
                    // If they haven't made the local stack big enough this could fail. Hope they know what they're doing
                    stack_set_value_at(destAddress, vm->registers[sourceReg], vm->functions[vm->fp].localStack, &okay);
                }
                assert(okay == STACK_OKAY);
                break;
            }          
            case INS_PUSH :
            {
                uint8_t destStack = util_extract_byte(ins, 6);
                uint8_t sourceReg = util_extract_byte(ins, 5);

                int okay = -255;
                if(destStack == GLOBAL_STACK)
                {
                    stack_push(vm->registers[sourceReg], vm->globalStack, &okay);
                }
                else if (destStack == LOCAL_STACK )
                {
                    stack_push(vm->registers[sourceReg], vm->functions[vm->fp].localStack, &okay);
                }
                assert(okay == STACK_OKAY);
                break;
            }          
            case INS_POP  :
            {
                uint8_t destReg     = util_extract_byte(ins, 6);
                uint8_t sourceStack = util_extract_byte(ins, 5);

                int okay = -255;
                if(sourceStack == GLOBAL_STACK)
                {
                    vm->registers[destReg] = stack_pop(vm->globalStack, &okay);
                }
                else if (sourceStack == LOCAL_STACK )
                {
                    vm->registers[destReg] = stack_pop(vm->functions[vm->fp].localStack, &okay);
                }
                
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Pop Result : %lu\n", vm->registers[destReg]);
#endif
                assert(okay == STACK_OKAY);
                break;
            }          
            case INS_SIZE :
            {
                uint8_t destReg         = util_extract_byte(ins, 6);
                uint8_t stackInQuestion = util_extract_byte(ins, 5);

                if(stackInQuestion == GLOBAL_STACK)
                {
                    vm->registers[destReg] = stack_get_size(vm->globalStack);
                }
                else if (stackInQuestion == LOCAL_STACK )
                {
                    vm->registers[destReg] = stack_get_size(vm->functions[vm->fp].localStack);
                }
                else
                {
                    printf("Invalid 'size' instruction!\n");
                    return VM_RUN_ERROR_UNKNOWN_INSTRUCTION;
                }
                
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Size Result : %lu\n", vm->registers[destReg]);
#endif
                break;
            }
            case INS_JUMP :
            {
                uint64_t destAddress = (uint64_t)util_extract_two_bytes(ins, 6) << 16| 
                                       (uint64_t)util_extract_two_bytes(ins, 4);

                vm->functions[vm->fp].ip = destAddress; 
                continue;
            }          

            case INS_CS_SF :
            {
                // Call Stack Store function ( The function to return to when next return hits)
                uint64_t func_from =  (uint64_t)util_extract_two_bytes(ins, 6) << 16| 
                                      (uint64_t)util_extract_two_bytes(ins, 4);

                int cssfOkay = 0;
                stack_push(func_from, vm->callStack, &cssfOkay);

                assert(cssfOkay == STACK_OKAY);
                break;
            }

            case INS_CS_SR :
            {
                // Call Stack Store Region Of Interest ( Instruction Pointer )
                uint64_t roi =  (uint64_t)util_extract_two_bytes(ins, 6) << 16| 
                                (uint64_t)util_extract_two_bytes(ins, 4);
                                    
                int cssrOkay = 0;
                stack_push(roi, vm->callStack, &cssrOkay);
                
                assert(cssrOkay == STACK_OKAY);
                break;
            }

            case INS_CALL :
            {
                // Call
                uint64_t destAddress =  (uint64_t)util_extract_two_bytes(ins, 6) << 16| 
                                        (uint64_t)util_extract_two_bytes(ins, 4);

                vm->fp = destAddress;

                currentFunction = &vm->functions[vm->fp];

                switchingFunction = 1;

                currentFunction->ip = 0;
                break;
            }          
            case INS_RET  :
            {
vm_attempt_force_return:
                if(stack_is_empty(vm->callStack))
                {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Callstack empty. Exiting\n");
#endif
                    FILE_GLOBAL_IS_VM_RUNNING = 0;
                    return 0;
                }

                int getRetData = 0;

                uint64_t ret_roi = stack_pop(vm->callStack, &getRetData);
                assert(getRetData == STACK_OKAY);

                uint64_t func_to = stack_pop(vm->callStack, &getRetData);
                assert(getRetData == STACK_OKAY);

                // Clear out the function's local call stack
                while(0 == stack_is_empty(currentFunction->localStack))
                {
                    int k; stack_pop(currentFunction->localStack, &k); assert(k == STACK_OKAY);
                }

                vm->fp = func_to;

                currentFunction = &vm->functions[vm->fp];

                currentFunction->ip = ret_roi;

                switchingFunction = 1;

                break; // Yes
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
                    FILE_GLOBAL_IS_VM_RUNNING = 0;
                    return 0;
                }
                return VM_RUN_ERROR_UNKNOWN_INSTRUCTION;
                break; 
            }
        }

        // Check action registers to see if a device needs to be called
        // ----------------------------------------------------------------------------

        if(vm->registers[10] != 0)
        {
            uint8_t device_id = util_extract_byte(vm->registers[10], 7);

            switch(device_id)
            {
            case NABLA_DEVICE_ADDRESS_IO:
                io_process(io_device, vm);
                break;

            case NABLA_DEVICE_ADDRESS_NET:
                printf("Network device has not yet been created\n");
                vm->registers[10] = 0;
                break;

            default:
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Invalid device id found in register 10.\n");
#endif
                return VM_RUN_ERROR_UNKNOWN_INSTRUCTION;
            };
        }

        // ----------------------------------------------------------------------------

        //  Increase the instruction pointer if we aren't explicitly told not to
        //
        if(0 == switchingFunction)
        {
            currentFunction->ip++;
        }
        else
        {
        // This was only to ensure we didn't inc the ip, and since we didn't we will un-flag this
        // so we can step through the next (funky fresh) function
            switchingFunction = 0;
        }
    }

    return 0;
}

// -----------------------------------------------------
//  We pass through cycle so we don't have to run
//  the init check every time when running a bin
// -----------------------------------------------------

int vm_step(struct VM* vm, uint64_t n)
{
    if(!FILE_GLOBAL_IS_VM_INITIALIZED)
    {
        return VM_RUN_ERROR_VM_NOT_INITIALIZED;
    }

    return vm_cycle(vm, n);
}

// -----------------------------------------------------
// Run a vm until the end of its life
// -----------------------------------------------------

int vm_run(NVM* vm)
{
    if(1 == FILE_GLOBAL_IS_VM_RUNNING)        {  return VM_RUN_ERROR_VM_ALREADY_RUNNING; }

    if(0 == FILE_GLOBAL_IS_VM_INITIALIZED)
    {
        int init_res = vm_init(vm);

        if(init_res != 0)
        {
            return init_res;
        }
    }

    // Indicate that it is now running
    FILE_GLOBAL_IS_VM_RUNNING = 1;

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
    printf("Running VM\n");
#endif

    int result;
    while(FILE_GLOBAL_IS_VM_RUNNING)
    {
        result = vm_cycle(vm, 1);

        if(result != 0)
        {
            return result;
        }
    }
    return result;
}