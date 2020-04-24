#include "../stack.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

// -----------------------------------------------
//                  PUSH POP TESTS
// -----------------------------------------------
void pushPop()
{
    int result;

    // Each stack frame is 8 bytes. 

    NablaStack stack = stack_new(100);

    assert(stack);

    assert(stack_is_empty(stack) == 1);

    // Push a bunch of values, and ensure that the stack grows
    for(uint64_t i = 0; i < 100; i++)
    {
        stack_push(i, stack, &result);              // Started off as size '1' so must grow by 1

        // We don't ouput as-to not spray the output
        assert((result == STACK_OKAY));
    }

    assert(stack_is_empty(stack) == 0);

    // Ensure that we can get all of the set values
    for(uint64_t i = 0; i < 100; i++)
    {
        uint64_t val_at = stack_value_at(i, stack, &result);

        assert((result == STACK_OKAY));
        assert((val_at == i));
    }

    // Ensure that we can pop all of the set values
    for(uint64_t i = 100; i > 0; i--)
    {
        int pop_result_code;
     
        uint64_t pop_val = stack_pop(stack, &pop_result_code);

        assert((pop_result_code == STACK_OKAY));
        assert((pop_val == i-1));
    }

    assert(stack_is_empty(stack) == 1);
}


// -----------------------------------------------
//                   MAIN
// -----------------------------------------------
int main(void)
{
    printf("Starting libc stack tests...");
    pushPop();
    printf("complete\n");
    exit(EXIT_SUCCESS);
}
