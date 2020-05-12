#ifndef NABLA_STACK_H
#define NABLA_STACK_H

#include <stdint.h>

#define STACK_OKAY                     0
#define STACK_EMPTY                    1
#define STACK_ERROR_IDX_OUT_OF_RANGE -10
#define STACK_FULL                   -20

//! \brief A stack
typedef struct Stack * NablaStack;

//! \brief Create a new stack
//! \param capacity Initial size of memory to reserve for stack. Must be > 0
//! \returns Stack pointer
NablaStack stack_new(uint64_t capacity);

//! \brief Delete a stack
//! \post All reserved stack information freed
void stack_delete(NablaStack stack);

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
//! \returns Value
int64_t stack_value_at(uint64_t pos, NablaStack stack, int* result);

//! \brief Get the element at the given position
//! \param[in] The position to put without pushing
//! \param[in] The value to set
//! \param[in] The stack
//! \param[out] result The result code of the operation
void stack_set_value_at(uint64_t pos, uint64_t val, NablaStack stack, int* result);

//! \brief Push value onto stack
//! \param[in] val The value to push onto the stack
//! \param[in] stack The stack to push to
//! \param[out] result The result code of the operation
void stack_push(int64_t val, NablaStack stack, int* result);

//! \brief Pop value off of stack
//! \param[in] stack The stack to pop from
//! \param[out] result The result code of the operation
//! \retval Value at stack top
int64_t stack_pop(NablaStack stack, int* result);

//! \brief Get a single byte out of the stack by index
//! \param[in] stack The stack to get byte from
//! \param[in] pos The position to get byte from
//! \param[out] result The result code of the operation
//! \returns Byte if index is valid
uint8_t stack_get_byte(NablaStack stack, uint64_t pos, int* result);

//! \brief Put a byte in the stack by index
//! \param[in] stack The stack to put byte into
//! \param[in] pos The position to put the byte
//! \param[in] daata The byte to place in the stack
//! \param[out] result The result code of the operation
//! \post If index is within range of stack size, the byte is placed in. If it exceeds
//!       current stack size, then the stack is expanded.
//!       If the index is greater than the capacity of the stack, result will indicate error.
void stack_put_byte(NablaStack stack, uint64_t pos, uint8_t data, int* result);

#endif