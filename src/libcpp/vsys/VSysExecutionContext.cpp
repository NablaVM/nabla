#include "VSysExecutionContext.hpp"
#include "VSysMachine.hpp"
#include <assert.h>

#include <iostream>

extern "C"
{
    #include "util.h"
}


namespace NABLA
{
namespace VSYS
{
    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    ExecutionContext::ExecutionContext(Machine &owner,
                                       uint64_t startAddress, 
                                       Memory<NABLA_VSYS_SETTINGS_GLOBAL_MEMORY_BYTES> &global_memory, 
                                       std::vector< std::vector<uint64_t> > &functions) :
                                                                                         owner(owner),
                                                                                         contextCompleted(false),
                                                                                         global_memory(global_memory),
                                                                                         functions(functions),
                                                                                         currentInstructionBlock(startAddress),
                                                                                         switchingFunction(false)

    {
        //  For HUGE projects that run on VSYS this will be a lot of overhead. It will have to iterate over all functions. If 
        //  theres a small number thats fine, but if its 10000+ it will be a headache. A 'loader' could be made that populates 
        //  the entry function, and populates 'on the fly' as things get called (if they get called)
        for(uint64_t i = 0; i < functions.size(); i++)
        {
            InstructionBlock ib;
            ib.instruction_pointer = 0;
            ib.instructions = &functions[i];
            contextFunctions.push_back(
                ib
            );
        }
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    ExecutionContext::~ExecutionContext()
    {

    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    bool ExecutionContext::isContextComplete() const
    {
        return contextCompleted;
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    ExecutionReturns ExecutionContext::stepExecution(uint64_t steps)
    {
        for(uint64_t cycle = 0; cycle < steps; cycle++)
        {
            uint64_t poi = this->contextFunctions[this->currentInstructionBlock].instruction_pointer;

            if(poi >=  this->contextFunctions[this->currentInstructionBlock].instructions->size())
            {
    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                printf("Current instruction stack empty, attempting to force a return.\n");
    #endif
                if(!attempt_return())
                {
                    this->contextCompleted = true;
                    return ExecutionReturns::ALL_EXECUTION_COMPLETE;
                }

                /*
                    If no more intructions, and we were able to return we will find ourselves here.
                    We treat this are like the bottom of the loop, but we don't check for hardware
                    execution because we are out of instructions.. so clearly nothing is going on 
                */
                finalize_cycle();
                continue;
            }

            uint64_t ins = this->contextFunctions[this->currentInstructionBlock].instructions->at(poi);
            
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
                    get_arith_lhs_rhs(id, ins, &lhs, &rhs);
                    this->registers[dest] = (lhs << rhs);

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("LSH : result: %ld\n", this->registers[dest]);
    #endif
                    break;
                } 

                case INS_RSH :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    get_arith_lhs_rhs(id, ins, &lhs, &rhs);
                    this->registers[dest] = (lhs >> rhs);

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("RSH : result: %ld\n", this->registers[dest]);
    #endif
                    break;
                } 

                case INS_AND :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    get_arith_lhs_rhs(id, ins, &lhs, &rhs);
                    this->registers[dest] = (lhs & rhs);
    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("AND : result: %ld\n", this->registers[dest]);
    #endif
                    break;
                } 

                case INS_OR  :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    get_arith_lhs_rhs(id, ins, &lhs, &rhs);
                    this->registers[dest] = (lhs | rhs);
    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("OR : result: %ld\n", this->registers[dest]);
    #endif
                    break;
                } 

                case INS_XOR :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    get_arith_lhs_rhs(id, ins, &lhs, &rhs);
                    this->registers[dest] = (lhs ^ rhs);
    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("XOR : result: %ld\n", this->registers[dest]);
    #endif
                    break;
                } 

                case INS_NOT :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    get_arith_lhs_rhs(id, ins, &lhs, &rhs);
                    this->registers[dest] = (~lhs);

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("NOT : result: %ld\n", this->registers[dest]);
    #endif
                    break;
                } 

                case INS_ADD  :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    assert(dest < 16);
                    get_arith_lhs_rhs(id, ins, &lhs, &rhs);
                    this->registers[dest] = lhs+rhs;

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("INS_ADD: result: %ld , DEST : %u\n", this->registers[dest], dest);
    #endif
                    break;
                }          
                case INS_SUB  :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    assert(dest < 16);
                    get_arith_lhs_rhs(id, ins, &lhs, &rhs);
                    this->registers[dest] = lhs-rhs;

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("INS_SUB: result: %ld , DEST : %u\n", this->registers[dest], dest);
    #endif
                    break;
                }          
                case INS_MUL  :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    assert(dest < 16);
                    get_arith_lhs_rhs(id, ins, &lhs, &rhs);
                    this->registers[dest] = lhs*rhs;

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("INS_MUL: result: %ld , DEST : %u\n", this->registers[dest], dest);
    #endif
                    break;
                }          
                case INS_DIV  :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    assert(dest < 16);
                    get_arith_lhs_rhs(id, ins, &lhs, &rhs);

                    if(rhs == 0){ this->registers[dest] = 0; }
                    else { this->registers[dest] = lhs/rhs; }
                    
    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("INS_DIV result: %ld , DEST : %u\n", this->registers[dest], dest);
    #endif
                    break;
                }          
                case INS_ADDD :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    assert(dest < 16);

                    get_arith_lhs_rhs(0, ins, &lhs, &rhs);
                
                    this->registers[dest] = util_convert_double_to_uint64( util_convert_uint64_to_double(lhs) + util_convert_uint64_to_double(rhs));

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Result : %f\n", util_convert_uint64_to_double(lhs) + util_convert_uint64_to_double(rhs));
    #endif
                    break;
                }          
                case INS_SUBD :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    assert(dest < 16);

                    get_arith_lhs_rhs(0, ins, &lhs, &rhs);
                
                    this->registers[dest] = util_convert_double_to_uint64( util_convert_uint64_to_double(lhs) - util_convert_uint64_to_double(rhs));

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Result : %f\n", util_convert_uint64_to_double(lhs) - util_convert_uint64_to_double(rhs));
    #endif
                    break;
                }          
                case INS_MULD :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    assert(dest < 16);

                    get_arith_lhs_rhs(0, ins, &lhs, &rhs);
                
                    this->registers[dest] = util_convert_double_to_uint64( util_convert_uint64_to_double(lhs) * util_convert_uint64_to_double(rhs));

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Result : %f\n", util_convert_uint64_to_double(lhs) * util_convert_uint64_to_double(rhs));
    #endif
                    break;
                }          
                case INS_DIVD :
                {
                    uint8_t dest =  util_extract_byte(ins, 6);
                    assert(dest < 16);

                    get_arith_lhs_rhs(0, ins, &lhs, &rhs);
                
                    this->registers[dest] = util_convert_double_to_uint64( util_convert_uint64_to_double(lhs) / util_convert_uint64_to_double(rhs));

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Result : %f\n", util_convert_uint64_to_double(lhs) / util_convert_uint64_to_double(rhs));
    #endif
                    break;
                }          
                case INS_BGT  :
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);
                    

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("BGT : %i\n", lhs > rhs);
    #endif

                    if(lhs > rhs){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BGTE :
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);
                    

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("BGTE : %i\n", lhs >= rhs);
    #endif

                    
                    if(lhs >= rhs){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BLT  :
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);
                    

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("BLT : %i\n", lhs < rhs);
    #endif

                    
                    
                    if(lhs < rhs){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BLTE :
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);
                    

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("BLTE : %i\n", lhs <= rhs);
    #endif

                    
                    
                    if(lhs <= rhs){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BEQ  :
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);

                    

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("BEQ : %i\n", lhs == rhs);
    #endif

                    
                    if(lhs == rhs){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BNE  :
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);
                    

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("BNE : %i\n", lhs != rhs);
    #endif

                    

                    if(lhs != rhs){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BGTD :
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    double lhs_d = util_convert_uint64_to_double(lhs);
                    double rhs_d = util_convert_uint64_to_double(rhs);

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);

                    if(lhs_d > rhs_d){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BGTED:
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    double lhs_d = util_convert_uint64_to_double(lhs);
                    double rhs_d = util_convert_uint64_to_double(rhs);

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);

                    if(lhs_d >= rhs_d){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BLTD :
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    double lhs_d = util_convert_uint64_to_double(lhs);
                    double rhs_d = util_convert_uint64_to_double(rhs);

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);

                    if(lhs_d < rhs_d){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BLTED:
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    double lhs_d = util_convert_uint64_to_double(lhs);
                    double rhs_d = util_convert_uint64_to_double(rhs);

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);

                    if(lhs_d <= rhs_d){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BEQD :
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    double lhs_d = util_convert_uint64_to_double(lhs);
                    double rhs_d = util_convert_uint64_to_double(rhs);

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);

                    if(util_check_double_equal(lhs_d, rhs_d)){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_BNED :
                {
                    lhs = this->registers[util_extract_byte(ins, 6)];
                    rhs = this->registers[util_extract_byte(ins, 5)];

                    double lhs_d = util_convert_uint64_to_double(lhs);
                    double rhs_d = util_convert_uint64_to_double(rhs);

                    uint64_t branchAddr = (uint64_t)util_extract_two_bytes(ins, 4) << 16 | 
                                          (uint64_t)util_extract_two_bytes(ins, 2);

                    if(!util_check_double_equal(lhs_d, rhs_d)){ this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = branchAddr; continue; }
                    break;
                }          
                case INS_MOV  :
                {
                    lhs = util_extract_byte(ins, 6);

                    // Move register value into another register
                    if(id == 0)
                    {
                        rhs = util_extract_byte(ins, 5);
                        this->registers[lhs] = this->registers[rhs];
                    }
                    // Move numerival value into a register
                    else if (id == 1)
                    {
                        int8_t rval = util_extract_byte(ins, 5);
                        this->registers[lhs] =  rval;
                    }
                    else
                    {
    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                        printf("Error: Unknown ID bits expressed in INS_MOV\n");
    #endif
                        return ExecutionReturns::UNKNOWN_INSTRUCTION;
                    }

    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("MOV result : %li\n", (int64_t)this->registers[lhs]);
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
                        sourceAddress = this->registers[sourceReg];
                    }
                    else
                    {
                        printf("Invalid 'ldb' instruction id : ID= %u\n", id);
                        return ExecutionReturns::UNKNOWN_INSTRUCTION;
                    }
                    
                    // Now that we have the source address and other required information, we can move on and actually do the load
                    bool okay = false;
                    uint8_t val;
                    if(stackSouce == GLOBAL_STACK)
                    {
                        okay = this->global_memory.get_8(sourceAddress, val);
                    }
                    else if ( stackSouce == LOCAL_STACK )
                    {
                        okay = this->contextFunctions[currentInstructionBlock].function_memory.get_8(
                            sourceAddress, val
                        );
                    }
                    assert(okay);
                    registers[dest] = val;

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Ldb Result: %lu\n", this->registers[dest]);
#endif
                    break;
                }
                case INS_LDW  :
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
                        sourceAddress = this->registers[sourceReg];
                    }
                    else
                    {
                        printf("Invalid 'ldw' instruction id : ID= %u\n", id);
                        return ExecutionReturns::UNKNOWN_INSTRUCTION;
                    }
                    
                    // Now that we have the source address and other required information, we can move on and actually do the load
                    bool okay = false;
                    uint64_t val;
                    if(stackSouce == GLOBAL_STACK)
                    {
                        okay = this->global_memory.get_64(sourceAddress, val);
                    }
                    else if ( stackSouce == LOCAL_STACK )
                    {
                        okay = this->contextFunctions[currentInstructionBlock].function_memory.get_64(
                            sourceAddress, val
                        );
                    }
                    assert(okay);
                    registers[dest] = val;

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Ldw Result: %lu\n", this->registers[dest]);
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
                        destAddress = this->registers[destReg];
                    }
                    else
                    {
                        printf("Invalid 'stb' instruction id : ID= %u\n", id);
                        return ExecutionReturns::UNKNOWN_INSTRUCTION;
                    }

                    // Now that we have the destination information, perform the store
                    bool okay = false;
                    uint8_t val =  registers[sourceReg] & 0x00000000000000FF;
                    if(stackDest == GLOBAL_STACK)
                    {
                        okay = this->global_memory.set_8(destAddress, val);
                    }
                    else if ( stackDest == LOCAL_STACK )
                    {
                        okay = this->contextFunctions[currentInstructionBlock].function_memory.set_8(
                            destAddress, val
                        );
                    }
                    assert(okay);
                    break;
                } 
                case INS_STW  :
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
                        destAddress = this->registers[destReg];
                    }
                    else
                    {
                        printf("Invalid 'stw' instruction id : ID= %u\n", id);
                        return ExecutionReturns::UNKNOWN_INSTRUCTION;
                    }

                    // Now that we have the destination information, perform the store
                    bool okay = false;
                    uint64_t val =  registers[sourceReg];


                    if(stackDest == GLOBAL_STACK)
                    {
                        okay = this->global_memory.set_64(destAddress, val);
                    }
                    else if ( stackDest == LOCAL_STACK )
                    {
                        okay = this->contextFunctions[currentInstructionBlock].function_memory.set_64(
                            destAddress, val
                        );
                    }
                    assert(okay);
                    break;
                }        
                case INS_PUSH  :
                {
                    uint8_t destStack = util_extract_byte(ins, 6);
                    uint8_t sourceReg = util_extract_byte(ins, 5);

                    bool okay = false;
                    if(destStack == GLOBAL_STACK)
                    {
                        okay = this->global_memory.push_8((this->registers[sourceReg] & 0x00000000000000FF));
                    }
                    else if (destStack == LOCAL_STACK )
                    {
                        okay = this->contextFunctions[currentInstructionBlock].function_memory.push_8(
                            (this->registers[sourceReg] & 0x00000000000000FF)
                        );
                    }
                    assert(okay);
                    break;
                }  
                case INS_PUSHW :
                {
                    uint8_t destStack = util_extract_byte(ins, 6);
                    uint8_t sourceReg = util_extract_byte(ins, 5);

                    bool okay = false;
                    if(destStack == GLOBAL_STACK)
                    {
                        okay = this->global_memory.push_64(this->registers[sourceReg]);
                    }
                    else if (destStack == LOCAL_STACK )
                    {
                        okay = this->contextFunctions[currentInstructionBlock].function_memory.push_64(
                            this->registers[sourceReg]
                        );
                    }
                    assert(okay);
                    break;
                }      
                case INS_POP  :
                {
                    uint8_t destReg     = util_extract_byte(ins, 6);
                    uint8_t sourceStack = util_extract_byte(ins, 5);

                    bool okay = false;
                    uint8_t val;
                    if(sourceStack == GLOBAL_STACK)
                    {
                        okay = this->global_memory.pop_8(val);
                    }
                    else if (sourceStack == LOCAL_STACK )
                    {
                        okay = this->contextFunctions[currentInstructionBlock].function_memory.pop_8(
                            val
                        );
                    }
                    
                    assert(okay);
                    this->registers[destReg] = val;
                    break;
                }    
                case INS_POPW  :
                {
                    uint8_t destReg     = util_extract_byte(ins, 6);
                    uint8_t sourceStack = util_extract_byte(ins, 5);

                    bool okay = false;
                    uint64_t val;
                    if(sourceStack == GLOBAL_STACK)
                    {
                        okay = this->global_memory.pop_64(val);
                    }
                    else if (sourceStack == LOCAL_STACK )
                    {
                        okay = this->contextFunctions[currentInstructionBlock].function_memory.pop_64(
                            val
                        );
                    }
                    
                    assert(okay);
                    this->registers[destReg] = val;
                    break;
                }          
                case INS_SIZE :
                {
                    uint8_t destReg         = util_extract_byte(ins, 6);
                    uint8_t stackInQuestion = util_extract_byte(ins, 5);

                    if(stackInQuestion == GLOBAL_STACK)
                    {
                        this->registers[destReg] = this->global_memory.getSize();
                    }
                    else if (stackInQuestion == LOCAL_STACK )
                    {
                        this->registers[destReg] = this->contextFunctions[
                            currentInstructionBlock
                            ].function_memory.getSize();
                    }
                    else
                    {
                        printf("Invalid 'size' instruction!\n");
                        return ExecutionReturns::UNKNOWN_INSTRUCTION;
                    }
                    
    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                    printf("Size Result : %lu\n", this->registers[destReg]);
    #endif
                    break;
                }
                case INS_JUMP :
                {
                    uint64_t destAddress = (uint64_t)util_extract_two_bytes(ins, 6) << 16| 
                                        (uint64_t)util_extract_two_bytes(ins, 4);

                    this->contextFunctions[
                        this->currentInstructionBlock
                        ].instruction_pointer = destAddress; 
                    continue;
                }          

                case INS_YIELD:
                {
                    if(this->callStack.empty())
                    {
    #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
                        printf("Callstack empty on yield. Exiting\n");
    #endif
                        this->contextCompleted = true;
                        return ExecutionReturns::OKAY;
                    }

                    uint64_t ret_roi = this->callStack.top(); this->callStack.pop();

                    uint64_t func_to = this->callStack.top(); this->callStack.pop();

                    // Increase the current instruction pointer so when we're called again we start off where we left

                    this->contextFunctions[currentInstructionBlock].instruction_pointer++;

                    this->currentInstructionBlock = func_to;

                    this->contextFunctions[currentInstructionBlock].instruction_pointer = ret_roi;

                    this->switchingFunction = true;

                    break;
                }

                case INS_CS_SF :
                {
                    // Call Stack Store function ( The function to return to when next return hits)
                    uint64_t func_from =  (uint64_t)util_extract_two_bytes(ins, 6) << 16| 
                                          (uint64_t)util_extract_two_bytes(ins, 4);

                    this->callStack.push(func_from);
                    break;
                }

                case INS_CS_SR :
                {
                    // Call Stack Store Region Of Interest ( Instruction Pointer )
                    uint64_t roi =  (uint64_t)util_extract_two_bytes(ins, 6) << 16| 
                                    (uint64_t)util_extract_two_bytes(ins, 4);
                                        
                    this->callStack.push(roi);
                    break;
                }

                case INS_CALL :
                {
                    // Call
                    uint64_t destAddress =  (uint64_t)util_extract_two_bytes(ins, 6) << 16| 
                                            (uint64_t)util_extract_two_bytes(ins, 4);

                    currentInstructionBlock = destAddress;

                    this->switchingFunction = true;
                    break;
                }       
                case INS_RET  :
                {
                    // Attempt a return. If it returns, true will be signaled
                    if(!attempt_return())
                    {
                        this->contextCompleted = true;
                        return ExecutionReturns::ALL_EXECUTION_COMPLETE;
                    }
                    break; // Yes
                }          
                case INS_EXIT :
                {
                    this->contextCompleted = true;
                    return ExecutionReturns::ALL_EXECUTION_COMPLETE;
                }    
                default:
                {
                    if(this->contextFunctions[currentInstructionBlock].instruction_pointer == 
                        this->contextFunctions[currentInstructionBlock].instructions->size())
                    {
                        this->contextCompleted = true;
                        return ExecutionReturns::OKAY;
                    }
                    return ExecutionReturns::UNKNOWN_INSTRUCTION;
                    break; 
                }
            }

            hardware_execution_check();

            finalize_cycle();
        }

        return ExecutionReturns::OKAY;
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    void ExecutionContext::finalize_cycle()
    {
        // ----------------------------------------------------------------------------

        //  Increase the instruction pointer if we aren't explicitly told not to
        //
        if(!this->switchingFunction)
        {
            this->contextFunctions[currentInstructionBlock].instruction_pointer++;
        }
        else
        {
        // This was only to ensure we didn't inc the ip, and since we didn't we will un-flag this
        // so we can step through the next (funky fresh) function
            this->switchingFunction = false;
        }
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    void ExecutionContext::hardware_execution_check()
    {
                // Check action registers to see if a device needs to be called
        // ----------------------------------------------------------------------------

        if(this->registers[10] != 0)
        {
            /*
                Devices take in both device pointer, AND VM, as the interface for the devices 
                has been set as-such to ensure that the device is logically seperated from the 
                machine. This gives us some flex for future work
            */
//             uint8_t device_id = util_extract_byte(this->registers[10], 7);
//
//             switch(device_id)
//             {
//             case VM_SETTINGS_DEVICE_ADDRESS_IO:
//                 io_process(this->io_device, vm);
//                 break;
//
//             case VM_SETTINGS_DEVICE_ADDRESS_NET:
//                 net_process(this->net_device, vm);
//                 break;
//
//             case VM_SETTINGS_DEVICE_ADDRESS_HOST:
//                 host_process(this->host_device, vm);
//                 break;
//
//             default:
// #ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
//                     printf("Invalid device id found in register 10.\n");
// #endif
//                 return VM_RUN_ERROR_UNKNOWN_INSTRUCTION;
//             };
        }
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    void ExecutionContext::get_arith_lhs_rhs(uint8_t id, uint64_t ins, int64_t * lhs, int64_t * rhs)
    {
        if(id == 0)
        {
            *lhs =   this->registers[util_extract_byte(ins, 5)];
            *rhs =   this->registers[util_extract_byte(ins, 4)];
        }
        else if (id == 1)
        {
            *lhs =  this->registers[util_extract_byte(ins, 5)];
            *rhs = (int16_t)util_extract_two_bytes(ins, 4);
        }
        else if (id == 2)
        {
            *lhs =  (int16_t)util_extract_two_bytes(ins, 5);
            *rhs =  this->registers[util_extract_byte(ins, 3)];
        }
        else if (id == 3)
        {
            *lhs =  (int16_t)util_extract_two_bytes(ins, 5);
            *rhs =  (int16_t)util_extract_two_bytes(ins, 3);
        }
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    bool ExecutionContext::attempt_return()
    {
        if(this->callStack.empty())
        {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
            printf("Callstack empty. Exiting\n");
#endif
            return false;
        }

        int getRetData = 0;

        uint64_t ret_roi = this->callStack.top(); this->callStack.pop();

        uint64_t func_to = this->callStack.top(); this->callStack.pop();

        // Clear out the function's local call stack
        while( this->contextFunctions[currentInstructionBlock].function_memory.hasData() )
        {
            uint8_t t;
            this->contextFunctions[currentInstructionBlock].function_memory.pop_8(t);
        }

        this->contextFunctions[currentInstructionBlock].instruction_pointer = 0;

        this->currentInstructionBlock = func_to;
        this->contextFunctions[currentInstructionBlock].instruction_pointer = ret_roi;
        this->switchingFunction = true;

        return true;
    }
}
}