
extern "C"
{
    #include "stack.h"
    #include "util.h"
}

#include <iostream>
#include <random>
#include "CppUTest/TestHarness.h"

namespace
{
    uint16_t getRandom16(uint16_t low, uint16_t high)
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(low, high);
        return dis(gen);
    }
}

TEST_GROUP(StackTests)
{   
};

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(StackTests, pushPopTests)
{
    for(int pptest = 0; pptest < 100; pptest++)
    {
        uint16_t stackSize = getRandom16(10, 50); // Random sized, kind of small. Don't want to kill travis-ci

        NablaStack stack = stack_new(stackSize);

        CHECK_FALSE(nullptr == stack);

        CHECK_EQUAL_TEXT(1, stack_is_empty(stack), "Stack did not initialize as empty");


        // Push a bunch of values, and ensure that the stack grows
        for(uint64_t i = 0; i < stackSize; i++)
        {
            int result;

            stack_push(i, stack, &result);

            CHECK_EQUAL_TEXT(STACK_OKAY, result, "A push did not return STACK_OKAY");
        }

        CHECK_EQUAL_TEXT(0, stack_is_empty(stack), "Populated stack reporting empty");

        // Ensure that we can get all of the set values
        for(uint64_t i = 0; i < stackSize; i++)
        {
            int result;

            uint64_t val_at = stack_value_at(i, stack, &result);

            CHECK_EQUAL_TEXT(STACK_OKAY, result, "A stack_value_at did not return STACK_OKAY");
            
            CHECK_EQUAL_TEXT(i, val_at, "The value at a stack index was not what we expected");
        }

        // Ensure that we can pop all of the set values
        for(uint64_t i = stackSize; i > 0; i--)
        {
            int result;
        
            uint64_t pop_val = stack_pop(stack, &result);

            CHECK_EQUAL_TEXT(STACK_OKAY, result, "A stack_pop did not return STACK_OKAY");

            CHECK_EQUAL_TEXT(i-1, pop_val, "Popped value was not what we expected");
        }

        CHECK_EQUAL_TEXT(1, stack_is_empty(stack), "Fully popped stack not reporting empty");

        stack_delete(stack);
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(StackTests, getByteTests)
{
    for(int pptest = 0; pptest < 100; pptest++)
    {
        uint16_t stackSize = getRandom16(10, 50); // Random sized, kind of small. Don't want to kill travis-ci

        NablaStack stack = stack_new(stackSize);

        CHECK_FALSE(nullptr == stack);

        CHECK_EQUAL_TEXT(1, stack_is_empty(stack), "Stack did not initialize as empty");


        // Push a bunch of values, and ensure that the stack grows
        for(uint64_t i = 0; i < stackSize; i++)
        {
            int result;

            stack_push(0, stack, &result);

            CHECK_EQUAL_TEXT(STACK_OKAY, result, "A push did not return STACK_OKAY");
        }

        CHECK_EQUAL_TEXT(0, stack_is_empty(stack), "Populated stack reporting empty");

        // Get a random index into a uint64
        uint8_t byteIndex = getRandom16(0, 7);

        // Generate a random byte that we want to search for
        uint8_t randomByte = getRandom16(0, 200);

        // Gen a random place in the stack to store the byte
        uint16_t frameIndex = getRandom16(0, stackSize-1);

        // Put the byte into a frame
        uint64_t frame = 0;
        frame |= ((uint64_t)randomByte << byteIndex * 8);
        
        // Sanity
        CHECK_EQUAL(randomByte, util_extract_byte(frame, byteIndex));

        // Store the frame in the stack
        int result = -255;
        stack_set_value_at(frameIndex, frame, stack, &result);
        CHECK_EQUAL_TEXT(STACK_OKAY, result, "Set value at failed");

        // Attempt to actually get the byte straight from the stack
        result = -255;

        //                                       8 bytes per idx + The index of the byte within the frame (7 being MSB)
        uint8_t byteGet = stack_get_byte(stack, (frameIndex * 8) + (7-byteIndex) , &result);

        CHECK_EQUAL_TEXT(STACK_OKAY, result, "A GET did not return STACK_OKAY");

        CHECK_EQUAL(randomByte, byteGet);

        stack_delete(stack);
    }
}

// ---------------------------------------------------------------
// 
// ---------------------------------------------------------------

TEST(StackTests, putByteTests)
{
    for(int pptest = 0; pptest < 100; pptest++)
    {
        NablaStack stack = stack_new(201);

        // Make a random byte
        uint8_t randomByte = getRandom16(0, 200);

        // Get a random index to place the byte
        uint16_t byte_index = getRandom16(10, 200*8);

        // Put the random byte in a random index
        int result = -255;
        stack_put_byte(stack, byte_index, randomByte , &result);

        CHECK_EQUAL_TEXT(STACK_OKAY, result, "A PUT did not return STACK_OKAY");

        uint64_t stack_size = stack_get_size(stack);

        // Make sure that the stack size expanded to fit whatever size was needed to store the byte index
        // we subtract 1 from stack size  because size is end.
        CHECK_EQUAL(uint64_t(byte_index/8), stack_size-1);

        // Get the random byte from the index
        result = -255;
        uint8_t getByte = stack_get_byte(stack, byte_index, &result);
        CHECK_EQUAL_TEXT(STACK_OKAY, result, "Set value get failed");

        // Ensure the byte we got is the byte that we put in
        CHECK_EQUAL(randomByte, getByte);

        stack_delete(stack);
    }
}