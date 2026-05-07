#ifndef STACK_H
#define STACK_H

#include <stddef.h>

typedef int type;
typedef struct Stack Stack;

typedef enum {
    STACK_OK = 0,
    STACK_RESIZE_ERR = 1,
    STACK_OVERFLOW_ERR = 1 << 1,
    STACK_EMPTY_ERR = 1 << 2
} StackError;

typedef enum {
    STACK_SUCCESS = 0,
    STACK_FAILURE = 1,
    STACK_NULL_POINTER_ERR = 2,
    STACK_ARG_NULL_POINTER_ERR = 3,
    STACK_INCORRECT_START_CAPACITY_ERR = 4
} StackStatus;

/** Allocates and initializes a stack. Returns NULL on failure or start_capacity < 1. */
Stack* init(size_t start_size);

/** Pushes value into the stack, resizing if needed. */
StackStatus push(Stack* stack, type value);

/** Removes the top element. Fails on empty stack. */
StackStatus pop(Stack* stack);

/** Writes the top element into *value without removing it. Fails on empty stack or NULL value. */
StackStatus top(Stack* stack, type* value);

/** Writes the current number of elements into *size. */
StackStatus size(Stack* stack, size_t* size);

/** Frees all memory. The pointer is invalid after this call. */
StackStatus clear(Stack* stack);

/** Returns 1 if empty, 0 if not. */
StackStatus empty(Stack* stack, int* out);

/** O(1). Writes 1 into *out if canaries are correct, 0 if corrupted. */
StackStatus check_canaries(Stack* stack, int* out);

/** O(n). Recomputes hash. Writes 1 into *out if valid, 0 if corrupted. */
StackStatus check_hash(Stack* stack, int* out);

/** Prints in stderr full stack data and params. */
StackStatus print_dump(Stack* stack);

#endif