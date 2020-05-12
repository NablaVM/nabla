#include "stack.h"
#include "util.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <math.h>

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

void stack_delete(NStack * stack)
{
    free(stack->elements);
    free(stack);
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
        return;
    }

    // Values exist at or below top, so we check range here
    if(stack->top < pos)
    {
        *result = STACK_ERROR_IDX_OUT_OF_RANGE;
        return;
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

    stack->empty = 0;
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

// -----------------------------------------------------
//
// -----------------------------------------------------

uint8_t stack_get_byte(NStack * stack, uint64_t pos, int* result)
{
    assert(stack);
    assert(result);

    uint64_t frame = floor(pos/8); // 0-indexed

    if(stack->top < frame)
    {
        *result = STACK_ERROR_IDX_OUT_OF_RANGE;
        return 0;
    }

    uint8_t  byteIndex = 7 - (pos % 8);

    *result = STACK_OKAY;

    return util_extract_byte(stack->elements[frame], byteIndex);
}

// -----------------------------------------------------
//
// -----------------------------------------------------

void stack_put_byte(NStack * stack, uint64_t pos, uint8_t data, int* result)
{
    assert(stack);
    assert(result);

    uint64_t frame = floor(pos/8); // 0-indexed

    if(stack->capacity < frame)
    {
        *result = STACK_ERROR_IDX_OUT_OF_RANGE;
        return;
    }

    // Expand the stack to ensure we can place the byte there
    if(stack->top < frame)
    {
        while(stack->top < frame)
        {
            int push_result = -255;
            stack_push(0, stack, &push_result);

            if(push_result != STACK_OKAY)
            {
                *result = push_result;
                return;
            }
        }
    }

    uint8_t  byteIndex = 7 - (pos % 8);

    // Clear the byte out 
    stack->elements[frame] |= ((uint64_t)(0xFF) << byteIndex * 8);

    // Add the new value in
    stack->elements[frame] &= ((uint64_t)data << byteIndex * 8);

    *result = STACK_OKAY;
}