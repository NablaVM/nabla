
extern "C"
{
    #include "stack.h"
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