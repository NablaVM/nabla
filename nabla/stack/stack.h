#ifndef NABLA_STACK_H
#define NABLA_STACK_H

#include <stdint.h>

//! \brief A stack
typedef struct Stack * NablaStack;

//! \brief Create a new stack
//! \param capacity Initial size of memory to reserve for stack. Must be > 0
//! \returns Stack pointer
NablaStack stack_new(uint64_t capacity);

//! \brief Get the capacity of the stack
//! \param stack The stack
//! \returns capacity of given stack
uint64_t stack_get_capacity(NablaStack stack);

//! \brief Get the size of the stack
//! \param stack The stack
//! \returns Number of elements (size) in stack
uint64_t stack_get_size(NablaStack stack);

//! \brief Check if the stack is empty
//! \param stack The stack
//! \returns 1 If empty, 0 otherwise
uint8_t stack_is_empty(NablaStack stack);

//! \brief Get the element at the given position
//! \param[in] The position to get from the stack without popping
//! \param[in] The stack
//! \param[out] result The result code of the operation
//! \returns Size of the stack
uint64_t stack_value_at(uint64_t pos, NablaStack stack, int* result);

//! \brief Push value onto stack
//! \param[in] val The value to push onto the stack
//! \param[in] stack The stack to push to
//! \param[out] result The result code of the operation
void stack_push(uint64_t val, NablaStack stack, int* result);

//! \brief Pop value off of stack
//! \param[in] stack The stack to pop from
//! \param[out] result The result code of the operation
//! \retval Value at stack top
uint64_t stack_pop(NablaStack stack, int* result);


#endif