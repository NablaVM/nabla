#include "../stack.h"
#include "../stackrc.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

// -----------------------------------------------
//                  PUSH POP TESTS
// -----------------------------------------------
void pushPop()
{
    printf("Push & pop tests...");
    int result;

    NablaStack stack = stack_new(1);

    assert(stack);

    assert(stack_is_empty(stack) == 1);

    // Push a bunch of values, and ensure that the stack grows
    for(uint8_t i = 0; i < 50; i++)
    {
        stack_push(i, stack, &result);              // Started off as size '1' so must grow by 1

        // We don't ouput as-to not spray the output
        assert((result == STACK_OKAY));


        if(i == 0)
            assert((stack_get_capacity(stack) == 1)); // Capacity 1 when nothing in it
        else if (i > 1)
            assert((stack_get_capacity(stack) == i)); // If we've gone over 1, it should expand
        
        assert((stack_get_size(stack) == i+1));     // How many it is holding
    }

    assert(stack_is_empty(stack) == 0);

    // Ensure that we can get all of the set values
    for(uint8_t i = 0; i < 50; i++)
    {
        uint8_t val_at = stack_value_at(i, stack, &result);

        assert((result == STACK_OKAY));
        assert((val_at == i));
    }

    // Ensure that we can pop all of the set values
    for(uint8_t i = 50; i > 0; i--)
    {
        int pop_result_code;
     
        uint8_t pop_val = stack_pop(stack, &pop_result_code);

        assert((pop_result_code == STACK_OKAY));
        assert((pop_val == i-1));

        assert((49 == stack_get_capacity(stack))); // Ensure capacity doesn't change


        if(i-1 > 0)
            assert((i-1 == stack_get_size(stack)));      // Ensure elements are being removed
        else 
            assert((0 == stack_get_size(stack)));       // Should be empty now
    }

    assert(stack_is_empty(stack) == 1);

    printf("complete\n");
}

// -----------------------------------------------
//                  GROW TESTS
// -----------------------------------------------
void grow()
{
    printf("grow tests...");

    NablaStack stack = stack_new(10);           // Create stack

    assert(stack);                              // Ensure its okay

    assert(stack_is_empty(stack) == 1);         // Ensure empty

    assert((10 == stack_get_capacity(stack)));  // Ensure size is what we told it

    int result;

    uint64_t saved_cap = 10;

    for(int i = 1; i <= 1000; i++)              // Grow the stack by 100 i-times 
    {
        stack_grow(100, stack, &result);

        assert(result == STACK_OKAY);           // Ensure no memory issues
        
        assert((10 + (i*100) == stack_get_capacity(stack))); // Is it actually getting bigger?
    
        saved_cap = stack_get_capacity(stack);
    }

    assert(stack_is_empty(stack) == 1);         // Big but should be empty

    for(uint64_t i = 0; i < stack_get_capacity(stack); i++) // Fill it up to capacity
    {
        stack_push(i, stack, &result);
        
        assert(result == STACK_OKAY);
    }

    assert(stack_is_empty(stack) == 0);        // Ensure filled

    assert(saved_cap == stack_get_capacity(stack));  // Ensure it didn't grow any

    while(!stack_is_empty(stack))
    {
        uint8_t unused = stack_pop(stack, &result);
    }

    assert(saved_cap == stack_get_capacity(stack));  // Ensure it didn't shrink any

    assert(stack_is_empty(stack) == 1);
    
    printf("complete\n");
}

// -----------------------------------------------
//                  SHRINK TESTS
// -----------------------------------------------
void shrink()
{
    printf("shrink tests...");

    NablaStack stack = stack_new(1000);

    assert((1000 == stack_get_capacity(stack)));
    
    int result;

    for(int i = 60; i < 71; i++)
    {
        stack_push(i, stack, &result);
        assert(result == STACK_OKAY); 
    }

    assert((1000 == stack_get_capacity(stack)));

    stack_shrink_to_fit(stack, &result);

    if(result == STACK_ERROR_MEM_REALLOC_FAIL)
    {
        printf("Memory error on shrink to fit\n");
        return;
    }

    assert(result == STACK_OKAY); 

    assert((10 == stack_get_capacity(stack)));

    for(int i = 0; i < 10; i++)
    {
        uint8_t val_at = stack_value_at(i, stack, &result);

        assert((result == STACK_OKAY));
        assert((val_at == 60 + i));
    }

    printf("complete\n");
}

// -----------------------------------------------
//                   MAIN
// -----------------------------------------------
int main(void)
{
    pushPop();

    grow();

    shrink();

    printf("\nAll tests completed\n");
    exit(EXIT_SUCCESS);
}