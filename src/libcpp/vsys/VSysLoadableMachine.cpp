#include "VSysLoadableMachine.hpp"
#include "VSysSettings.hpp"
#include "VSysInstructions.hpp"

#include <assert.h>
#include <iostream>

namespace NABLA
{
namespace VSYS
{
    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------
 
    LoadableMachine::LoadableMachine()
    {

    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    LoadableMachine::~LoadableMachine()
    {

    }

    void LoadableMachine::load_numerical_constant(FILE* file, uint8_t nBytes, int *result)
    {
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

        if(!this->global_memory.push_64(stackValue))
        {
            *result = -1;
            return;
        }

        *result = 0;

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
        std::cout << "numerical constant loaded : " << stackValue << std::endl;
#endif
    }

    // -----------------------------------------------------
    //
    // -----------------------------------------------------

    void LoadableMachine::load_string_constant(FILE* file, int *result)
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
            std::cout << (char)currentChunk;
#endif
            shifter--;

            // If we are below 0 that means this stack value is full
            // we need to reset the shifter, and push the value onto the stack
            if(shifter < 0 || i == strSize-1)
            {
                shifter = 7;

                if(!this->global_memory.push_64(stackValue))
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
        std::cout << std::endl;
#endif
    }

    // -----------------------------------------------------
    //
    // -----------------------------------------------------

    int LoadableMachine::load_constant(FILE * file, int currentByte)
    {
        uint8_t op = (currentByte & 0xFC);
        uint8_t id = (currentByte & 0x03);

        if(op == CONST_INT)
        {
            int result = 0;
            if(id == 0) { /* .int8  */ load_numerical_constant(file, 1, &result); }
            if(id == 1) { /* .int16 */ load_numerical_constant(file, 2, &result); }
            if(id == 2) { /* .int32 */ load_numerical_constant(file, 4, &result); }
            if(id == 3) { /* .int64 */ load_numerical_constant(file, 8, &result); }

            if(result != 0) { return -1; }
        }

        if(currentByte == CONST_DBL)
        {
            // Load 8 bytes
            int result = 0;
            load_numerical_constant(file, 8, &result);

            if(result != 0) { return -1; }
        }

        if(currentByte == CONST_STR)
        {
            int result = 0;
            load_string_constant(file, &result);

            if(result != 0) { return -1; }
        }

        return 0;
    }

    // -----------------------------------------------------
    //
    // -----------------------------------------------------

    int LoadableMachine::load_end_of_binary(FILE * file)
    {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
        std::cout << "load_end_of_binary" << std::endl;
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

    int LoadableMachine::load_function(FILE* file)
    {
#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
        std::cout << "load_function" << std::endl;
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

        std::vector<uint64_t> currentFunctionIns;

        // All instructions are 8 bytes, so the number of instructions
        // is the bytes divided by 8
        uint64_t insCount = insBytes/8;

#ifdef NABLA_VIRTUAL_MACHINE_DEBUG_OUTPUT
        std::cout << "There are : " << insCount << " instructions in the function" << std::endl;
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
                }
                else
                {
                    return -1;
                }
            }
            currentFunctionIns.push_back(currentIns);   
        }

        this->functions.push_back(currentFunctionIns);

        this->numberOfFunctions++;
        return 0;
    }

    // ----------------------------------------------------------------
    //
    // ----------------------------------------------------------------

    LoadableMachine::LoadResultCodes LoadableMachine::loadFile(std::string path)
    {
        FILE * file = fopen(path.c_str(), "rb");

        if(file == NULL)
        {
            std::cerr << "LoadableMachine : Unable to open file " << path << std::endl;
            return LoadResultCodes::UNABLE_TO_OPEN_FILE;
        }

        uint8_t currentByte;

        enum LoadStatus
        {
            IDLE,
            EXPECT_CONSTANT,
            EXPECT_FUCNTION,
            STOP_READING
        };

        LoadStatus currentStatus = IDLE;

        this->numberOfFunctions = 0;
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
                            fclose(file);
                            return LoadResultCodes::ERROR_FAILED_TO_LOAD_CONSTANTS;
                        }
                    }

                    // If there are no constants present, then okay, move on. 
                    if (numberOfConstants == 0)
                    {
                        currentStatus = IDLE;
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
                            fclose(file);
                            return LoadResultCodes::ERROR_FAILED_TO_LOAD_FUCNTION;
                        }
                    }
                }
                else
                {
                    fclose(file);
                    return LoadResultCodes::ERROR_UNHANDLED_INSTRUCTION;
                }
            }

            // Start off assuming that we are getting constants
            // The way that the solace parser works is it stuffs all constants first 
            // in the order that they are defined in the asm file 
            else if(currentStatus == EXPECT_CONSTANT)
            {
                // Chop off the bottom 2 bits to see what operation is indicated
                uint8_t op = (currentByte & 0xFC);

                // If the instruction is to load a constant, call the constant loaders that will
                // consume that constant up-until the next instruction
                if(op == CONST_INT || currentByte == CONST_DBL || currentByte == CONST_STR)
                {
                    if( 0 != load_constant(file, currentByte) )
                    {
                        fclose(file);
                        return LoadResultCodes::ERROR_FAILED_TO_LOAD_CONSTANTS;
                    }

                    // Dec the number of expected constants
                    numberOfConstants--;
                }
                else
                {
                    fclose(file);
                    return LoadResultCodes::ERROR_UNHANDLED_INSTRUCTION;
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
                    
                    // Need to eat the entire eof instruction
                    for(int c = 0; c < 7; c++)   
                    {
                        uint8_t tmp = 0;
                        if(1 != fread(&tmp, 1, 1, file))
                        {
                            fclose(file);
                            return LoadResultCodes::ERROR_FAILED_TO_LOAD_FUCNTION;
                        }
                    }
                }
                else if(currentByte == INS_FUNCTION_CREATE)
                {
                    if(0 != load_function(file))
                    { 
                        fclose(file);
                        return LoadResultCodes::ERROR_FAILED_TO_LOAD_FUCNTION; 
                    }
                }
                else if (currentByte == INS_SEG_BEOF)
                {
                    if(0 != load_end_of_binary(file))
                    {
                        fclose(file);
                        return LoadResultCodes::ERROR_FAILED_TO_LOAD_FUCNTION; 
                    }

                    // Set the state to end. 
                    currentStatus = STOP_READING;
                }
                else
                {
                    fclose(file);
                    return LoadResultCodes::ERROR_UNHANDLED_INSTRUCTION;
                }
            }
        } 

        //  Ensure that we stopped reading because we wanted to
        //
        if(currentStatus != STOP_READING)
        {
            fclose(file);
            return LoadResultCodes::ERROR_EOB_NOT_FOUND;
        }

        assert(expectedEntryAddress < NABLA_VSYS_SETTINGS_MAX_FUNCTIONS);

        //  Set the entry address 
        //
        this->entryAddress = expectedEntryAddress;

        fclose(file);
        return LoadResultCodes::OKAY;
    }
    
}
}