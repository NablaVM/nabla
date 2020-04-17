#include "../stack.h"
#include "../stackrc.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

void check_result(int result)
{
    switch(result)
    {
        case STACK_OKAY:                    printf("success\n");                      return;
        case STACK_ERROR_IDX_OUT_OF_RANGE:  perror("STACK_ERROR_IDX_OUT_OF_RANGE\n"); exit(EXIT_FAILURE); break;
        case STACK_ERROR_MEM_REALLOC_FAIL:  perror("STACK_ERROR_MEM_REALLOC_FAIL\n");   exit(EXIT_FAILURE); break;
        default:
            perror("Unknown return result\n");
            exit(EXIT_FAILURE);
            break;
    }
}

// -----------------------------------------------
//                  PUSH POP TESTS
// -----------------------------------------------
void pushPop()
{
    printf("Push & pop tests...");
    int result;

    NablaStack stack = stack_new(1);

    assert(stack);

    // Push a bunch of values, and ensure that the stack grows
    for(uint8_t i = 0; i < 50; i++)
    {
        stack_push(i, stack, &result);              // Started off as size '1' so must grow by 1

        // We don't ouput as-to not spray the output
        assert((result == STACK_OKAY));
        assert((stack_get_capacity(stack) == i+1)); // How many it can hold
        assert((stack_get_size(stack) == i+1));     // How many it is holding
    }

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

        assert((50 == stack_get_capacity(stack))); // Ensure capacity doesn't change


        if(i-1 > 0)
            assert((i-1 == stack_get_size(stack)));      // Ensure elements are being removed
        else 
            assert((1 == stack_get_size(stack)));       // There should always be '1'
    }






    printf("complete\n");
}

// -----------------------------------------------
//                  GROW TESTS
// -----------------------------------------------
void grow()
{

}

// -----------------------------------------------
//                   MAIN
// -----------------------------------------------
int main(void)
{
    pushPop();

    grow();

    printf("complete\n");
    exit(EXIT_SUCCESS);
}