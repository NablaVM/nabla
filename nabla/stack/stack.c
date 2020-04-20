#include "stackrc.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

/*
    [ ------------------ elements ------------------]
     _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _
    | | | | | | | | | | | | | | | | | | | | | | | | |
    |5|6|3|1|.|.|.|9|8| | | | | | | | | | | | | | | |
    |_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|_|
                     /                             /
             top -- /               capacity -----/
*/

struct Stack
{
    uint8_t  empty : 1;     // Marker if its 'empty'
    uint64_t top;           // top of stack
    uint64_t capacity;      // num reserved elementss
    int64_t *elements;      // Actual elements
};

typedef struct Stack NStack;

// -----------------------------------------------------
//
// -----------------------------------------------------

NStack * stack_new(uint64_t capacity)
{
    assert(capacity > 0);

    NStack * stack = (NStack*)malloc(sizeof(NStack));

    stack->elements = (uint64_t *)malloc(sizeof(uint64_t) * capacity);

    assert(stack->elements);

    stack->top   = 0;
    stack->empty = 1;
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

    return (stack->empty == 1) ? 0 : stack->top+1;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

uint8_t stack_is_empty(NStack * stack)
{
    assert(stack);

    return stack->empty;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

int64_t stack_value_at(uint64_t pos, NStack * stack, int* result)
{
    assert(stack);
    assert(result);

    if(stack->empty)
    {
        *result = STACK_ERROR_IDX_OUT_OF_RANGE;
        return 0;
    }

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

void stack_set_value_at(uint64_t pos, uint64_t val, NStack * stack, int* result)
{
    assert(stack);
    assert(result);
    
    if(stack->empty)
    {
        *result = STACK_ERROR_IDX_OUT_OF_RANGE;
        return 0;
    }

    // Values exist at or below top, so we check range here
    if(stack->top < pos)
    {
        *result = STACK_ERROR_IDX_OUT_OF_RANGE;
        return 0;
    }

    stack->elements[pos] = val;
    
    *result = STACK_OKAY;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

void stack_push(int64_t val, NStack * stack, int* result)
{
    assert(stack);
    assert(result);

    // If we need to grow to support the push, then grow
    if(stack->capacity == stack->top)
    {
        *result = STACK_FULL;
        return;
    }

    if(stack->empty == 1)
    {
        stack->empty = 0;
        stack->elements[stack->top] = val;
    }
    else
    {
        stack->top += 1;
        stack->elements[stack->top] = val;
    }

    *result = STACK_OKAY;
}

// -----------------------------------------------------
//
// -----------------------------------------------------

int64_t stack_pop(NStack * stack, int* result)
{
    assert(stack);
    assert(result);

    // If top is spot 0, we dont dec. We just erase it.
    if(stack->empty == 1)
    {
        *result = STACK_EMPTY;
        return 0;
    }

    int64_t val = stack->elements[stack->top];

    if(stack->top == 0)
    {
        stack->empty = 1;
    }
    else
    {
        stack->top -= 1;
    }

    *result = STACK_OKAY;

    return val;
}