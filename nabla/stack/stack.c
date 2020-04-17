#include "stackrc.h"

#include <stdlib.h>
#include <stdint.h>
#include <assert.h>

/*

    [ ------------------ elements ------------------]
     _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
    | | | | | | | | | | | | | | | | | | | | | | | | |
    |5|6|3|1|.|.|.|9| | | | | | | | | | | | | | | | |
    |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
                     /                             /
             top -- /                   size -----/

    Top is technically the top of the stack+1 i.e the next area that something
    would be pushed to

*/

struct Stack
{
    uint64_t top;   // top of stack
    uint64_t capacity;  // num reserved elementss

    uint8_t *elements;
};

typedef struct Stack NStack;

// -----------------------------------------------------
//
// -----------------------------------------------------

NStack * stack_new(uint64_t capacity)
{
    assert(capacity > 0);

    NStack * stack = (NStack*)malloc(sizeof(NStack));

    stack->elements = (uint8_t *)malloc(sizeof(uint8_t) * capacity);

    assert(stack->elements);

    stack->top  = 0;
    stack->capacity = capacity;

    return stack;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

uint64_t stack_get_capacity(NStack * stack)
{
    assert(stack);

    return stack->capacity;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

uint64_t stack_get_size(NStack * stack)
{
    assert(stack);

    return (stack->top == 0) ? 1 : stack->top;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

uint8_t stack_value_at(uint64_t pos, NStack * stack, int* result)
{
    assert(stack);
    assert(result);

    // Values exist at or below top, so we check range here
    if(stack->top < pos)
    {
        *result = STACK_ERROR_IDX_OUT_OF_RANGE;
        return 0;
    }

    *result = STACK_OKAY;

    return stack->elements[pos];
}


// -----------------------------------------------------
//
// -----------------------------------------------------

void stack_grow(uint64_t size, NStack * stack, int* result)
{
    assert(stack);
    assert(result);

    uint8_t * tmp =  (uint8_t*)realloc(stack->elements, (sizeof(uint8_t) * stack->capacity) + size);

    if(tmp == NULL) { *result = STACK_ERROR_MEM_REALLOC_FAIL; return; }

    stack->elements = tmp;
    stack->capacity = stack->capacity + size;

    tmp = NULL;

    *result = STACK_OKAY;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

void stack_push(uint8_t val, NStack * stack, int* result)
{
    assert(stack);
    assert(result);

    // If we need to grow to support the push, then grow
    if(stack->capacity == stack->top)
    {
        int grow_result;

        stack_grow(1, stack, &grow_result);

        // If grow fails, indicate as such
        if(grow_result != STACK_OKAY) { *result = grow_result; return; }
    }

    stack->elements[stack->top] = val;
    stack->top += 1;

    *result = STACK_OKAY;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

uint8_t stack_pop(NStack * stack, int* result)
{
    assert(stack);
    assert(result);

    uint8_t value = 0;

    // If top is spot 0, we dont dec. We just erase it.
    if(stack->top == 0)
    {
        value = stack->elements[stack->top];
        stack->elements[stack->top] = 0;
    }
    else
    {
        value = stack->elements[stack->top-1];
        stack->top -= 1;
    }

    *result = STACK_OKAY;

    return value;
}